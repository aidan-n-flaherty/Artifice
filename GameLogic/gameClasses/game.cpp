#include <cstdlib>
#include <iostream>
#include <ctime>
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include <tuple>
#include <random>
#include <cmath>
#include "game.h"
#include "gameObjects/vessel.h"
#include "order.h"
#include "orders/send_order.h"
#include "orders/gift_order.h"
#include "orders/hire_order.h"
#include "orders/reroute_order.h"
#include "orders/promote_order.h"
#include "orders/release_order.h"
#include "orders/surrender_order.h"
#include "orders/mine_order.h"
#include "helpers/point.h"
#include "events/send_event.h"
#include "events/reroute_event.h"
#include "events/outpost_range_event.h"
#include "events/battle_event.h"
#include "events/win_condition_event.h"
#include "events/vessel_outpost_event.h"
#include "game_settings.h"

/* The game constructor should create the entire starting state deterministically based on
** the random seed provided.
*/
Game::Game(GameSettings settings, int simulatorID, double startTime, double endTime, std::map<int, std::tuple<std::string, int, int>> &playerInfo, int seed, bool cacheEnabled) :
    startTime(startTime), stateTime(startTime), endTime(endTime), cacheEnabled(cacheEnabled) {
    std::srand(seed);

    this->settings = new GameSettings(settings);

    std::vector<int> playerIDs;

    for(int i = 0; i < playerInfo.size(); i++) {
        auto [name, userID, rating] = playerInfo[i];
        int ID = incrementObjCounter();
        playerIDs.push_back(ID);
        Player* p = new Player(ID, getSettings(), name, userID, rating);
        if(userID == simulatorID) this->simulatorID = p->getID();
        addPlayer(p);
    }

    std::mt19937 gen(seed); //seeding the random number generator. Each time this value is referenced, a new random value
                            //is generated from the given seed

    //check if the current GameSettings has number_of_teams initialized to some value that is > 1
    if(this->settings->number_of_teams > 1 && players.size() % this->settings->number_of_teams == 0){
        for(int i = 0; i < players.size(); i++) players[i]->setTeam(i / this->settings->number_of_teams);

        for(int i = players.size() - 1; i >= 0; i--) {
            int j = gen() % players.size();

            int teamA = players[i]->getTeamID();
            players[i]->setTeam(players[j]->getTeamID());
            players[j]->setTeam(teamA);
        }

        if(this->settings->gameMode == Mode::CONQUEST) this->settings->gameMode = Mode::ELIMINATION;
    } else {
        this->settings->number_of_teams = -1;
    }

    // start map generation

    int totalOutposts = playerInfo.size() * this->settings->outpostsPerPlayer;

    this->settings->width = int(this->settings->defaultSonar * 0.75 * sqrt(totalOutposts));
    this->settings->height = int(this->settings->defaultSonar * 0.75 * sqrt(totalOutposts));

    int numIterations = 100;
    double epsilon = 0.0001; //small distance used to check if bases are on top of each other. prevents divide by 0 errors.

    std::vector<Point> outpostPositions;

    std::unordered_map<int, std::vector<OutpostType>> outpostTypes;

    //going through a assigning each player a starting position

    //bounds for uniform distribution, used for generating random width and height coordinates for each outpost

    for(int i = 0; i < (playerIDs.size()) * this->settings->outpostsPerPlayer; i++) {
        double x = (gen() % (this->settings->width * 100000)) / 100000.0;
        double y = (gen() % (this->settings->height * 100000)) / 100000.0;

        std::cout << x << ", " << y << std::endl;

        Point pos = Point(this->settings, x, y);
        
        //pos.constrain();

        outpostPositions.push_back(pos);

    }
    //------------------------------------------------------------------------------------

    //iterates through the outposts of each player
    for(int i = 0; i < numIterations; i++) {
        double mag = 20.0 - (20.0 * i) / numIterations;

        double forceX[outpostPositions.size()];
        double forceY[outpostPositions.size()];

        for(int j = 0; j < outpostPositions.size(); j++) {
            forceX[j] = 0;
            forceY[j] = 0;
        }

        for(int j = 0; j < outpostPositions.size(); j++) {
            for(int k = j + 1; k < outpostPositions.size(); k++) {
                if(j == k) continue;

                Point& a = outpostPositions[j];
                Point& b = outpostPositions[k];

                double dist = a.closestDistance(b);

                if(dist < epsilon) {
                    double angle = atan2(cos(i + j), sin(i + j));
                    a.set(a.getX() + epsilon * cos(angle), a.getY() + epsilon * sin(angle));
                }

                Point diff = a - a.closest(b);
                Point force = diff.normalized(mag/(0.01 * dist * dist + 1));

                forceX[j] += force.getX();
                forceY[j] += force.getY();

                forceX[k] -= force.getX();
                forceY[k] -= force.getY();
            }
            //all of the outpost points are now pushed away from each other. all outposts are
            //in equilibrium. For now, we are going to assume this acts as a uniform distribution
        }

        for(int j = 0; j < outpostPositions.size(); j++) {
            outpostPositions[j].set(outpostPositions[j].getX() + forceX[j], outpostPositions[j].getY() + forceY[j]);
        }
    }



    /* Both of these functions should be moved to after everything else:
    */
    
    int numPlayers = players.size();
    
    std::vector<Point> current_centroids;

    //initalize the first centroid positions
    while(current_centroids.size() < numPlayers){
        double x = (gen() % (this->settings->width * 100000)) / 100000.0;
        double y = (gen() % (this->settings->height * 100000)) / 100000.0;

        //generates a new centroid randomly using the previously specified width and height distribution bounds
        Point new_centroid = Point(this->settings, x, y);

        //marker to check to make sure that the centroid does not yet exist
        bool centroid_exists = false;

        for(auto point : current_centroids){

            if(point == new_centroid){
                centroid_exists = true;
            }

        }
        //if the newly generated centroid doesn't exist, then add it to the current centroids vector
        if(centroid_exists == false){
            current_centroids.push_back(new_centroid);
        }
    }

    std::vector<std::tuple<double, double, int>> curr_centroid_data; //parallel vector to current_centroids.
                                                       //in the same index as it's corresponding centroid in current_centroids,
                                                       //stores (sum_of_all_x, sum_of_all_y, num_of_points)


    //storing empty tuples in curr_centroid_data to be edited in the new centroid position calculations
    for(int i = 0; i < current_centroids.size(); i++){
        curr_centroid_data.push_back(std::tuple<double, double, int>());
    }


    //begin the k-mean clustering
    for(int i = 0; i < numIterations; i++){
        /*loop numIteration times

        loop through each outpost location, and compare the minDistance from each centroid.
        for the centroid it is closest to, increment the number of points in centroid's sector,
        sum of all x coordinates, and sum of all y coordinates values in the centroid outpost counts map.
        
        Use the values currently in the vector of the centroid_outpost_counts to calculate a new centroid
        location using the mean (average x and average y) of all other points.

        */
        
        //resetting the stored data in curr_centroid data, so that a new average position
        //can be calculated for each centroid

        for(int i = 0; i < curr_centroid_data.size(); i++){

            std::get<0>(curr_centroid_data[i]) = 0.0; //setting average x coordinate to zero
            std::get<1>(curr_centroid_data[i]) = 0.0; //setting average y coordinate to zero
            std::get<2>(curr_centroid_data[i]) = 0; //setting number of outposts that are closest to it to zero

        }

        for(auto curr_point : outpostPositions){

            int closest_centroid_index = -1;
            double dist_from_closest_centroid = 1000000000000000; //arbitrarily large value

            //loops through and finds the index of the centroid with the least distance from curr_point
            for(int i = 0; i < current_centroids.size(); i++){

                if(curr_point.closestDistance(current_centroids[i]) < dist_from_closest_centroid){
                    closest_centroid_index = i;
                    dist_from_closest_centroid = curr_point.closestDistance(current_centroids[i]);
                }

            }

            if(closest_centroid_index == -1){
                //error message, in case that all of the centroids are somehow invalid. Should never trigger.
                std::cout << "Error in Map Generation: No Valid Centroids" << std::endl;
            }
            else{
                //edit data for corresponding tuple in curr_centroid_data

                //adjusting the sum of all x coordinates

                double old_x = std::get<0>(curr_centroid_data[closest_centroid_index]);
                std::get<0>(curr_centroid_data[closest_centroid_index]) = old_x + (current_centroids[closest_centroid_index].closest(curr_point)).getX();

                //adjusting the sum of all y coordinates

                double old_y = std::get<1>(curr_centroid_data[closest_centroid_index]);
                std::get<1>(curr_centroid_data[closest_centroid_index]) = old_y + (current_centroids[closest_centroid_index].closest(curr_point)).getY();

                //adding 1 to the total number of points closest to this centroid

                int old_count = std::get<2>(curr_centroid_data[closest_centroid_index]);
                std::get<2>(curr_centroid_data[closest_centroid_index]) = old_count + 1;

                //remember to put blank tuples into the vector, so that they can be accessed by the necessary index!
                //also remember to move the commented out loops to after all outposts have been initialized to a player
            }
        }
        
        //calculate new positions for all of the centroids

        for(int i = 0; i < current_centroids.size(); i++){

            double sum_x = std::get<0>(curr_centroid_data[i]);
            double sum_y = std::get<1>(curr_centroid_data[i]);
            int num_of_outposts = std::get<2>(curr_centroid_data[i]);

            double average_x = sum_x / num_of_outposts;
            double average_y = sum_y / num_of_outposts;

            current_centroids[i].setX(average_x);
            current_centroids[i].setY(average_y);
        }


    }

    //all centroids should now be in place: figure out the closest amount (half of total outposts per player) and set them for the player

    std::vector<OutpostType> types;

    //assigns (factoryDensity * OutpostsPerPlayer) factory types to each player, and fills the rest of their outpost types with
    //generators
    for(int j = 0; j < getSettings()->outpostsPerPlayer; j++) {
        if(gen() % 2 == 0) {
            if(j < getSettings()->factoryDensity * getSettings()->outpostsPerPlayer) types.push_back(OutpostType::FACTORY);
            else types.push_back(OutpostType::GENERATOR);
        } else {
            if(j < getSettings()->factoryDensity * getSettings()->outpostsPerPlayer) types.insert(types.begin(), OutpostType::FACTORY);
            else types.insert(types.begin(), OutpostType::GENERATOR);
        }
    }

    //this is gonna be a bit messy to read. The following is a hashmap that holds an ordered map
    //of all of the closest points to each players' centroid. The key of the hashmap corresponds
    //to the player to whom this centroid is assigned
    std::unordered_map<int, std::map<double, Point>> grouped_outpost_positions;


    //since current_centroids should be the same size as playerIDs, this loop declaration should work
    //initializing all of the maps for each centroid
    for(int i = 0; i < current_centroids.size(); i++){
        grouped_outpost_positions[i] = std::map<double, Point>();
    }

    for(auto pos : outpostPositions){

        int closest_centroid = -1; //the index of the closest centroid
        double least_distance = 10000000000; //arbitrarily large value, should always be replaced

        for(int i = 0; i < current_centroids.size(); i++){
            if(pos.closestDistance(current_centroids[i]) < least_distance){
                closest_centroid = i;
                least_distance = pos.closestDistance(current_centroids[i]);
            }
        }

        //if there is already some outpost stored that is exactly that distance away from the centroid.
        //prevents the map from overriding any outposts. technically could cause weirdness, but the chance
        //should be astronomically small.
        while(grouped_outpost_positions[closest_centroid].find(least_distance) != grouped_outpost_positions[closest_centroid].end()){
            least_distance += 0.00000000000000001; //adds the smallest possible value a standard double can hold.
        }

        grouped_outpost_positions[closest_centroid].insert({least_distance, pos});

    }

    for(int i = 0; i < outpostPositions.size(); i++) {
        Outpost* o = new Outpost(incrementObjCounter(), getSettings(), OutpostType::BROKEN, 10, outpostPositions[i].getX(), outpostPositions[i].getY());
        addOutpost(o);
    }

    for(int i = 0; i < this->settings->outpostsPerPlayer; i++) {
        for(int j = 0; j < numPlayers; j++) {
            Player* p = getPlayer(j);

            const Point& point = current_centroids[j];

            double closestDistance = -1;
            Outpost* closest = nullptr;
            for(auto& pair : getOutposts()) {
                if(pair.second->hasOwner() || pair.second->getType() != OutpostType::BROKEN) continue;

                double dist = point.closestDistance(pair.second->getPosition());
                if(closest == nullptr || dist < closestDistance) {
                    closestDistance = dist;
                    closest = pair.second;
                }
            }

            if(closest) {
                if(p->getOutposts().empty()) {
                    closest->setUnits(60);
                    p->addOutpost(closest);
                    
                    Specialist* s = new Specialist(incrementObjCounter(), getSettings(), SpecialistType::QUEEN);
                    addSpecialist(s);
                    p->addSpecialist(getSpecialist(s->getID()));
                    closest->addSpecialist(getSpecialist(s->getID()));
                } else if(p->getOutposts().size() < this->settings->outpostsPerPlayer / 2) {
                    closest->setUnits(40);
                    p->addOutpost(closest);

                    std::cout << j << " has an outpost at " << closest->getPosition().getX() << ", " << closest->getPosition().getY() << " with ID = " << closest->getID() << std::endl;
                }

                closest->setType(types[i]);
            }
        }
    }

    // //initializes the starter outposts as objects and assigns them the queen specialist
    // for(const std::pair<int, Point>& pair : startingPositions) {
    //     Outpost* o = new Outpost(incrementObjCounter(), getSettings(), OutpostType::FACTORY, 20, pair.second.getX(), pair.second.getY());
    //     addOutpost(o);
    //     getPlayer(pair.first)->addOutpost(getOutpost(o->getID()));

    //     Specialist* s = new Specialist(incrementObjCounter(), getSettings(), SpecialistType::QUEEN);
    //     addSpecialist(s);
    //     getPlayer(pair.first)->addSpecialist(getSpecialist(s->getID()));
    //     getPlayer(pair.first)->getOutposts().front()->addSpecialist(getSpecialist(s->getID()));
    // }
    

    // //iterates through the rest of outpost positions and outpost types and initializes all other outposts as objects
    // //and their specialists
    // for(const std::pair<int, Point>& pair : outpostPositions) {
    //     OutpostType type = outpostTypes[pair.first].front();
    //     outpostTypes[pair.first].erase(outpostTypes[pair.first].begin());

    //     Outpost* o = new Outpost(incrementObjCounter(), getSettings(), type, 20, pair.second.getX(), pair.second.getY());
    //     addOutpost(o);
    //     getPlayer(pair.first)->addOutpost(getOutpost(o->getID()));

    //     std::cout << pair.first << " has an outpost at " << o->getPosition().getX() << ", " << o->getPosition().getY() << " with ID = " << o->getID() << std::endl;
    // }

    // end map generation

    /*
    
    -generate all outpost points first
    -each should be on average ~ 0.75x the sonar range away from each other
    -assign the points that are closest to the calculated starting points to be the starting outposts of each
    -player
    

    */

    addEvent(new OutpostRangeEvent(getTime()));
}

Game::Game(const Game& game) : startTime(game.startTime), stateTime(game.stateTime), cacheEnabled(game.cacheEnabled), ended(game.ended), endTime(game.endTime), referenceID(game.referenceID), simulatorID(game.simulatorID), lastExecutedOrder(game.lastExecutedOrder), nextEndState(game.nextEndState), gameEndTime(game.gameEndTime), gameObjCounter(game.gameObjCounter), settings(game.settings) {
    for(Event* event : game.events) events.insert(event->copy());
    for(Event* event : game.simulatedEvents) simulatedEvents.push_back(event->copy());
    for(const auto& pair : game.vessels) vessels[pair.first] = new Vessel(*pair.second);
    for(const auto& pair : game.players) players[pair.first] = new Player(*pair.second);
    for(const auto& pair : game.outposts) outposts[pair.first] = new Outpost(*pair.second);
    for(const auto& pair : game.specialists) specialists[pair.first] = new Specialist(*pair.second);

    for(Event* event : events) event->updatePointers(this);
    for(const auto& pair : vessels) pair.second->updatePointers(this);
    for(const auto& pair : players) pair.second->updatePointers(this);
    for(const auto& pair : outposts) pair.second->updatePointers(this);
    for(const auto& pair : specialists) pair.second->updatePointers(this);

    orders = game.orders;
    invalidOrders = game.invalidOrders;
    cache = game.cache;
}

Game::~Game() {
    for(Event* event : events) delete event;
    for(Event* event : simulatedEvents) delete event;
    for(const auto& pair : vessels) delete pair.second;
    for(const auto& pair : players) delete pair.second;
    for(const auto& pair : outposts) delete pair.second;
    for(const auto& pair : specialists) delete pair.second;

    if(cache.size() == 0) {
        for(Order* o : orders) delete o;
        for(Order* o : invalidOrders) delete o;
        delete settings;
    }
}

void Game::removeRelevant(int id) {
    for (auto it = events.begin(); it != events.end();) {
        Event* e = *it;
        if(e->referencesObject(id)) {
            events.erase(it++);
            delete e;
        } else ++it;
    }
}

void Game::updateEvents() {
    // delete all objects flagged for deletion
    for(auto itA = vessels.begin(); itA != vessels.end();) {
        Vessel* vessel = itA->second;

        if(vessel->isDeleted()) {
            removeRelevant(vessel->getID());
            itA = vessels.erase(itA);

            for(auto& pair : vessels) {
                if(pair.second->getTargetID() == vessel->getID()) {
                    pair.second->returnHome();
                }
            }

            delete vessel;
        } else itA++;
    }

    for(auto itA = specialists.begin(); itA != specialists.end();) {
        Specialist* specialist = itA->second;

        if(specialist->isDeleted()) {
            itA = specialists.erase(itA);
            delete specialist;
        } else itA++;
    }

    // All players flagged for update (e.g. increased mining rate)
    for(auto itA = players.begin(); itA != players.end(); itA++) {
        Player* player = itA->second;

        if(!player->needsRefresh()) continue;

        removeRelevant(player->getID());

        player->projectedVictory(player, stateTime, events);
    }

    // All outposts flagged for update (e.g. changed specialists)
    /*for(auto itA = outposts.begin(); itA != outposts.end(); itA++) {
        Outpost* outpost = itA->second;

        if(!outpost->needsRefresh()) continue;

        removeRelevant(outpost->getID());
    }*/

    // All vessels flagged for update (e.g. global speed change) may have different combat times.
    for(auto itA = vessels.begin(); itA != vessels.end(); itA++) {
        Vessel* vessel = itA->second;

        if(!vessel->needsRefresh()) continue;

        removeRelevant(vessel->getID());

        for(auto itB = vessels.begin(); itB != vessels.end(); itB++) {
            if(itA == itB) continue;
            Vessel* otherVessel = itB->second;

            if(otherVessel->needsRefresh()) continue;
            
            vessel->collision(vessel, otherVessel, stateTime, events);
        }

        for(auto itB = outposts.begin(); itB != outposts.end(); itB++) {
            Outpost* outpost = itB->second;

            vessel->collision(vessel, outpost, stateTime, events);
        }

        vessel->setRefresh(false);
    }

    cacheState();
}

void Game::updateState(double timestamp) {
    double secondsElapsed = timestamp - stateTime;

    if(secondsElapsed == 0) return;

    for(auto& pair : vessels) pair.second->update(secondsElapsed);
    for(auto& pair : outposts) pair.second->update(secondsElapsed);
    for(auto& pair : players) pair.second->update(secondsElapsed);

    stateTime = timestamp;
}

void Game::cacheState() {
    if(cacheEnabled) cache.insert(std::shared_ptr<Game>(new Game(*this)));
}

/* Only run this function when the game has finished.
** Returns a list of player IDs, ordered by their final score.
** Returns an empty list if the game is still in progress.
** Fills up the inputted list of invalid orders with orders that were rejected.
*/
std::list<std::pair<int, int>> Game::run(bool pastEnd) {
    updateEvents();

    bool ranOutOfTime = false;

    // loops until no events or orders remain.
    // note that events will always be run before orders given the same timestamp.
    while(!events.empty() || !orders.empty()) {
        std::multiset<Event*>::iterator event = events.begin();

        // check if the next chronological event occurs after the next chronological order, and if so,
        // run the order instead of the event

        while(!orders.empty() && (events.empty() || (*event)->getTimestamp() > (*orders.begin())->getTimestamp())) {
            std::multiset<Order*>::iterator orderIt = orders.begin();
            Order* order = *orderIt;

            if(order->getTimestamp() < stateTime) {
                orders.erase(orderIt);
                invalidOrders.push_back(order);
                continue;
            }

            // need an updated state to check for order validity
            updateState(order->getTimestamp());

            // converts a order to an event
            orders.erase(orderIt);
            Event* converted = order->convert(this);

            if(order->getID() > referenceID && order->getSenderID() != simulatorID) referenceID = order->getID();
            lastExecutedOrder = order->getID();

            if(!converted) {
                invalidOrders.push_back(order);
            } else {
                event = events.insert(converted);
                break;
            }
        }

        // if orders is full of invalid orders and events is empty, this will stop a nullptr dereference
        if(events.empty()) break;
        
        Event* e = *event;

        events.erase(event);

        if(e->getTimestamp() <= endTime && dynamic_cast<OutpostRangeEvent*>(e) && vessels.empty()) {
            if(!orders.empty() || !events.empty()) addEvent(new OutpostRangeEvent(e->getTimestamp() + getSettings()->fireRate * getSettings()->baseFireRate / getSettings()->simulationSpeed));
            continue;
        }

        nextEndState = e->getTimestamp();

        if(e->getTimestamp() > endTime) {
            ranOutOfTime = true;
            break;
        }

        updateState(e->getTimestamp());

        e->run(this);
        simulatedEvents.push_back(e);

        if(!pastEnd && ended) break;

        updateEvents();
    }

    if(!ranOutOfTime) nextEndState = std::numeric_limits<double>::max();

    return getScores();
}

std::vector<Player*> Game::sortedPlayers() const {
    std::vector<Player*> scores;
    for(auto& pair : getPlayers()) scores.push_back(pair.second);

    switch(settings->gameMode) {
    case Mode::MINING:
        std::sort(scores.begin(), scores.end(), [](Player* a, Player* b) {
            return a->getResources() != b->getResources() ? a->getResources() > b->getResources()
                : a->hasLost() != b->hasLost() ? !a->hasLost()
                : !a->hasLost() && a->getUnits() != b->getUnits() ? a->getUnits() > b->getUnits()
                : a->hasLost() && a->getDefeatedTime() != b->getDefeatedTime() ? a->getDefeatedTime() > b->getDefeatedTime()
                : a->getID() > b->getID();
        });

        break;
    case Mode::CONQUEST: case Mode::ELIMINATION:
        std::sort(scores.begin(), scores.end(), [](Player* a, Player* b) {
            return a->getOutposts().size() != b->getOutposts().size() ? a->getOutposts().size() > b->getOutposts().size()
                : a->hasLost() != b->hasLost() ? !a->hasLost()
                : !a->hasLost() && a->getUnits() != b->getUnits() ? a->getUnits() > b->getUnits()
                : a->hasLost() && a->getDefeatedTime() != b->getDefeatedTime() ? a->getDefeatedTime() > b->getDefeatedTime()
                : a->getID() > b->getID();
        });
        
        break;
    }

    if(settings->number_of_teams > 1) {
        std::vector<Player*> newScores;

        while(!scores.empty()) {
            int teamID = scores.front()->getTeamID();

            for(int i = 0; i < scores.size(); i++) {
                if(scores[i]->getTeamID() == teamID) newScores.push_back(scores[i]);
            }

            for(int i = scores.size() - 1; i >= 0; i--) {
                if(scores[i]->getTeamID() == teamID) scores.erase(scores.begin() + i);
            }
        }

        return newScores;
    }

    return scores;
}

bool Game::hasEnded() const {
    std::vector<Player*> sorted = sortedPlayers();
    std::unordered_set<int> teams;

    // eliminating all other players always ends the game
    int alive = 0;
    for(const auto& pair : getPlayers()) {
        // only person with a queen is the winning player
        if(!pair.second->hasLost()) alive++;
        teams.insert(pair.second->getTeamID());
    }
    if(alive <= 1 || (teamGame() && teams.size() <= 1)) return true;

    switch(settings->gameMode) {
    case Mode::MINING:
        return sorted.front()->getResources() >= settings->resourcesToWin;
    case Mode::CONQUEST: {
        int outpostsToWin = (1 + (getPlayers().size() / 2)) * settings->outpostsPerPlayer;

        return sorted.front()->getOutposts().size() >= outpostsToWin;
    }
    default: return false;
    }

    return true;
}

void Game::endGame() {
    gameEndTime = stateTime;
    ended = true;
}

std::list<std::pair<int, int>> Game::getScores() {
    std::list<std::pair<int, int>> scores;

    std::vector<Player*> sorted = sortedPlayers();

    if(!hasEnded()) return scores;

    for(int i = 0; i < sorted.size(); i++) {
        // 1st place has a score of 1, last place has a score of 0, linear interpolation for all in between
        double score = (sorted.size() - 1.0 - i) / (sorted.size() - 1.0);
        double scoreDelta = 0;

        for(int j = 0; j < sorted.size(); j++) {
            if(i == j) continue;

            double expected = 1.0 / (1 + pow(10, (sorted[j]->getRating() - sorted[i]->getRating())/400.0));
            scoreDelta += settings->eloKValue * (score - expected);
        }

        scores.push_back(std::make_pair<int, int>(sorted[i]->getUserID(), round(scoreDelta / (sorted.size() - 1))));
    }

    return scores;
}

std::shared_ptr<Game> Game::lastState(double timestamp) {
    std::shared_ptr<Game> returnVal = shared_from_this();

    if(cache.begin() != cache.end()) returnVal = *cache.begin();
    else return returnVal;
    for(auto it = cache.begin(); it != cache.end(); it++) {
        if((*it)->getTime() > timestamp) break;
        else returnVal = *it;
    }

    return returnVal;
}

double Game::nextState(double timestamp) {
    double returnTime = timestamp;

    for(auto it = cache.begin(); it != cache.end(); it++) {
        returnTime = (*it)->getTime();
        if((*it)->getTime() > timestamp) break;
    }

    return returnTime <= timestamp ? std::numeric_limits<double>::max() : returnTime;
}

Event* Game::nextAssociatedEvent(double timestamp, int id) {
    for(Event* event : simulatedEvents) {
        if(event->getTimestamp() > timestamp && event->referencesObject(id)) return event;
    }

    return nullptr;
}

// method should only be called by the final game state
const BattleEvent* Game::nextBattle(int id, double timestamp) {
    for(Event* event : simulatedEvents) {
        if(event->getTimestamp() > timestamp && event->referencesObject(id)) {
            BattleEvent* b = dynamic_cast<BattleEvent*>(event);
            if(b && !b->isFriendly()) return b;
        }
    }

    return nullptr;
}

const WinConditionEvent* Game::nextWinCondition(double timestamp) {
    for(Event* event : simulatedEvents) {
        if(event->getTimestamp() > timestamp) {
            WinConditionEvent* w = dynamic_cast<WinConditionEvent*>(event);
            if(w) return w;
        }
    }

    return nullptr;
}

const VesselOutpostEvent* Game::nextArrival(int id, double timestamp) {
    for(Event* event : simulatedEvents) {
        if(event->getTimestamp() > timestamp && event->referencesObject(id)) {
            VesselOutpostEvent* b = dynamic_cast<VesselOutpostEvent*>(event);
            if(b) return b;
        }
    }

    return nullptr;
}

// method should only be called by the current game state, and returned battles are only predictions
std::list<BattleEvent*> Game::nextBattles(int id) {
    std::list<BattleEvent*> l;

    for(Event* event : events) {
        if(event->referencesObject(id)) {
            BattleEvent* b = dynamic_cast<BattleEvent*>(event);
            if(b) l.push_back(b);
        }
    }

    return l;
}

// assuming the client is looking at forecasted battles, this finds the battles that actually end up taking place
const BattleEvent* Game::simulatedBattle(int eventID) {
    for(Event* event : simulatedEvents) {
        if(event->getID() == eventID) {
            BattleEvent* b = dynamic_cast<BattleEvent*>(event);
            if(b) return b;
        }
    }

    return nullptr;
}

std::shared_ptr<Game> Game::removeOrder(int ID) {
    std::shared_ptr<Game> returnVal = shared_from_this();

    for(auto it = cache.begin(); it != cache.end(); it++) {
        if((*it)->getLastExecutedOrder() == ID) break;
        else returnVal = *it;

        for(Order* o : returnVal->getOrders()) {
            if(o->getID() == ID) {
                returnVal->removeOrder(o);
                break;
            }
        }
    }

    return returnVal;
}

void Game::addOrder(const std::string &type, int ID, int referenceID, double timestamp, int senderID, int arguments[], int argCount) {
    double time = timestamp;

    std::list<int> argumentIDs;
    for(int i = 0; i < argCount; i++) argumentIDs.push_back(arguments[i]);

    if(type == "SEND" && argumentIDs.size() >= 3) {
        int numUnits = argumentIDs.front();
        argumentIDs.pop_front();
        int originID = argumentIDs.front();
        argumentIDs.pop_front();
        int targetID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new SendOrder(ID, time, senderID, numUnits, argumentIDs, originID, targetID, referenceID));
    } else if(type == "HIRE" && argumentIDs.size() >= 1) {
        int specialistID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new HireOrder(ID, time, senderID, specialistID, referenceID));
    } else if(type == "RELEASE" && argumentIDs.size() >= 1) {
        int specialistID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new ReleaseOrder(ID, time, senderID, specialistID, referenceID));
    } else if(type == "GIFT" && argumentIDs.size() >= 1) {
        int vesselID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new GiftOrder(ID, time, senderID, vesselID, referenceID));
    } else if(type == "PROMOTE" && argumentIDs.size() >= 2) {
        int specialistID = argumentIDs.front();
        argumentIDs.pop_front();
        int promoteID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new PromoteOrder(ID, time, senderID, specialistID, promoteID, referenceID));
    } else if(type == "REROUTE" && argumentIDs.size() >= 2) {
        int vesselID = argumentIDs.front();
        argumentIDs.pop_front();
        int targetID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new RerouteOrder(ID, time, senderID, vesselID, targetID, referenceID));
    } else if(type == "MINE" && argumentIDs.size() >= 1) {
        int outpostID = argumentIDs.front();
        argumentIDs.pop_front();
        addOrder(new MineOrder(ID, time, senderID, outpostID, referenceID));
    } else if(type == "SURRENDER") {
        addOrder(new SurrenderOrder(ID, time, senderID, referenceID));
    } else {
        std::cout << "Unknown order type" << std::endl;
    }
}

std::shared_ptr<Game> Game::processOrder(const std::string &type, int ID, int referenceID, double timestamp, int senderID, int arguments[], int argCount) {
    std::list<int> argumentIDs;
    for(int i = 0; i < argCount; i++) argumentIDs.push_back(arguments[i]);

    std::shared_ptr<Game> game = lastState(timestamp);

    for(Order* o : orders) {
        if(o->getID() == ID) return game;
    }

    game->addOrder(type, ID, referenceID, timestamp, senderID, arguments, argCount);

    return game;
}

void Game::addPlayer(Player* p) {
    players[p->getID()] = p;
}

void Game::addVessel(Vessel* v) {
    vessels[v->getID()] = v;
}

void Game::addOutpost(Outpost* o) {
    outposts[o->getID()] = o;
}

void Game::addSpecialist(Specialist* s) {
    specialists[s->getID()] = s;
}

void Game::addOrder(Order* o) {
    for(std::shared_ptr<Game> game : cache) game->orders.insert(o);

    orders.insert(o);
}

void Game::addEvent(Event* e) {
    events.insert(e);
}

void Game::addNotification(AttackNotification* n) {
    notifications.insert(n);
}


void Game::removeVessel(Vessel* v) {
    if(v->hasOwner()) v->getOwner()->removeVessel(v);
    while(!v->getSpecialists().empty()) removeSpecialist(v->getSpecialists().front());
    v->remove();
}

void Game::removeSpecialist(Specialist* s) {
    if(s->hasOwner()) s->getOwner()->removeSpecialist(s);
    if(s->getContainer()) s->getContainer()->removeSpecialist(s);
    s->remove();
}

void Game::removeOrder(Order* o) {
    orders.erase(o);
}

std::list<Outpost*> Game::getTeamOutposts(int teamID) const {
    std::list<Outpost*> returnVal;

    for(auto& pair : players) {
        if(pair.second->getTeamID() == teamID) {
            std::list<Outpost*> playerOutposts = pair.second->getOutposts();
            returnVal.insert(returnVal.end(), playerOutposts.begin(), playerOutposts.end());
        }
    }

    return returnVal;
}


std::list<Vessel*> Game::getTeamVessels(int teamID) const {
    std::list<Vessel*> returnVal;

    for(auto& pair : players) {
        if(pair.second->getTeamID() == teamID) {
            std::list<Vessel*> playerVessels = pair.second->getVessels();
            returnVal.insert(returnVal.end(), playerVessels.begin(), playerVessels.end());
        }
    }

    return returnVal;
}

bool Game::withinRange(Player* p, PositionalObject* obj, double timeDiff) const {
    if(obj->getOwnerID() == p->getID() || (teamGame() && obj->hasOwner() && obj->getOwner()->getTeamID() == p->getTeamID())) return true;

    std::list<Outpost*> controlledOutposts = teamGame() ? getTeamOutposts(p->getTeamID()) : p->getOutposts();
    std::list<Vessel*> controlledVessels = teamGame() ? getTeamVessels(p->getTeamID()) : p->getVessels();

    Vessel* v = dynamic_cast<Vessel*>(obj);

    for(Outpost* o : controlledOutposts) {
        if(o->getPositionAt(timeDiff).closestDistance(obj->getPositionAt(timeDiff)) < o->getSonarRange()) return true;

        if(v && v->getTargetID() == o->getID()) return true;
    }

    if(v) {
        for(Vessel* v1 : controlledVessels) {
            if(v->getTargetID() == v1->getID()) return true;
        }
    }

    return false;
}

bool GameOrder::operator()(const std::shared_ptr<Game> &lhs, const std::shared_ptr<Game> &rhs) const {
    double diff = lhs->getTime() - rhs->getTime();
    return diff == 0 ? lhs->simulatedEventCount() - rhs->simulatedEventCount() < 0 : diff < 0;
}