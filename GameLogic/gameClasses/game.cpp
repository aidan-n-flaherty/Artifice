#include <cstdlib>
#include <iostream>
#include <ctime>
#include <set>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>
#include <tuple>
#include <random>
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

    // start map generation

    int numIterations = 100;
    double epsilon = 0.0001;

    std::vector<std::pair<int, Point>> startingPositions;
    std::vector<std::pair<int, Point>> outpostPositions;

    std::unordered_map<int, std::vector<OutpostType>> outpostTypes;

    int n = 0;
    for(int id : playerIDs) {
        double angle = 2 * acos(-1) * (n++) / players.size();

        Point pos = Point(this->settings, this->settings->width/2 + 10 * cos(angle), this->settings->height/2 + 10 * sin(angle));
        pos.constrain();

        startingPositions.push_back(std::make_pair(id, pos));
        
        std::vector<OutpostType> types;
        for(int j = 0; j < getSettings()->outpostsPerPlayer; j++) {
            if(j < getSettings()->factoryDensity * getSettings()->outpostsPerPlayer) types.push_back(OutpostType::FACTORY);
            else types.push_back(OutpostType::GENERATOR);
        }
        //std::shuffle(types.begin(), types.end(), std::default_random_engine(seed));

        outpostTypes[id] = types;
    }

    for(int i = 0; i < numIterations; i++) {
        double mag = 10.0 - (10.0 * i) / numIterations;
        for(int j = 0; j < startingPositions.size(); j++) {
            for(int k = j + 1; k < startingPositions.size(); k++) {
                if(j == k) continue;

                Point& a = startingPositions[j].second;
                Point& b = startingPositions[k].second;

                double dist = a.closestDistance(b);

                if(dist < epsilon) {
                    double angle = atan2(cos(i + j), sin(i + j));
                    a.set(a.getX() + epsilon * cos(angle), a.getY() + epsilon * sin(angle));
                }

                Point newA = a.movedTowards(a.closest(b), -mag/(0.01 * dist + 1));
                Point newB = b.movedTowards(b.closest(a), -mag/(0.01 * dist + 1));

                a = newA;
                b = newB;
            }
        }
    }

    for(auto& pair : startingPositions) {
        for(int i = 0; i < getSettings()->outpostsPerPlayer; i++) {
            double angle = 2 * acos(-1) * i / getSettings()->outpostsPerPlayer;

            Point pos = Point(getSettings(), pair.second.getX() + 20 * cos(angle), pair.second.getY() + 20 * sin(angle));
            pos.constrain();

            outpostPositions.push_back(std::make_pair(pair.first, pos));
        }
    }

    for(int i = 0; i < numIterations; i++) {
        double mag = 10.0 - (10.0 * i) / numIterations;
        for(int j = 0; j < outpostPositions.size(); j++) {
            for(int k = 0; k < startingPositions.size(); k++) {
                Point& a = outpostPositions[j].second;
                const Point& b = startingPositions[k].second;

                double dist = a.closestDistance(b);

                if(dist < epsilon) {
                    double angle = atan2(cos(i + j), sin(i + j));
                    a.set(a.getX() + epsilon * cos(angle), a.getY() + epsilon * sin(angle));
                }

                if(dist > 50) continue;

                a.moveTowards(a.closest(b), -mag/(0.25 * dist + 1));
            }

            for(int k = j + 1; k < outpostPositions.size(); k++) {
                if(j == k) continue;

                Point& a = outpostPositions[j].second;
                Point& b = outpostPositions[k].second;

                double dist = a.closestDistance(b);

                if(dist < epsilon) {
                    double angle = atan2(cos(i + j), sin(i + j));
                    a.set(a.getX() + epsilon * cos(angle), a.getY() + epsilon * sin(angle));
                }

                if(dist > 50) continue;

                Point newA = a.movedTowards(a.closest(b), -mag/(0.25 * dist + 1));
                Point newB = b.movedTowards(b.closest(a), -mag/(0.25 * dist + 1));

                a = newA;
                b = newB;
            }
        }
    }

    for(const std::pair<int, Point>& pair : startingPositions) {
        Outpost* o = new Outpost(incrementObjCounter(), getSettings(), OutpostType::FACTORY, 20, pair.second.getX(), pair.second.getY());
        addOutpost(o);
        getPlayer(pair.first)->addOutpost(getOutpost(o->getID()));

        Specialist* s = new Specialist(incrementObjCounter(), getSettings(), SpecialistType::QUEEN);
        addSpecialist(s);
        getPlayer(pair.first)->addSpecialist(getSpecialist(s->getID()));
        getPlayer(pair.first)->getOutposts().front()->addSpecialist(getSpecialist(s->getID()));
    }

    for(const std::pair<int, Point>& pair : outpostPositions) {
        OutpostType type = outpostTypes[pair.first].front();
        outpostTypes[pair.first].erase(outpostTypes[pair.first].begin());

        Outpost* o = new Outpost(incrementObjCounter(), getSettings(), type, 20, pair.second.getX(), pair.second.getY());
        addOutpost(o);
        getPlayer(pair.first)->addOutpost(getOutpost(o->getID()));

        std::cout << pair.first << " has an outpost at " << o->getPosition().getX() << ", " << o->getPosition().getY() << " with ID = " << o->getID() << std::endl;
    }

    // end map generation

    addEvent(new OutpostRangeEvent(getTime()));
}

Game::Game(const Game& game) : startTime(game.startTime), stateTime(game.stateTime), cacheEnabled(game.cacheEnabled), endTime(game.endTime), referenceID(game.referenceID), simulatorID(game.simulatorID), lastExecutedOrder(game.lastExecutedOrder), nextEndState(game.nextEndState), gameObjCounter(game.gameObjCounter), settings(game.settings) {
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
        if((*it)->referencesObject(id)) events.erase(it++);
        else ++it;
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
                if(pair.second->getTarget()->getID() == vessel->getID()) {
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
std::list<std::pair<int, int>> Game::run() {
    updateEvents();

    bool ranOutOfTime = false;

    // loops until no events or orders remain.
    // note that events will always be run before orders given the same timestamp.
    while((!events.empty() || !orders.empty()) && !ended) {
        std::cout<<events.size()<<" events and "<<orders.size()<<" orders remain to be tracked..."<<std::endl;
        std::multiset<Event*>::iterator event = events.begin();

        /*for(auto it = events.begin(); it != events.end(); it++) {
            std::cout << (*it)->getTimestamp() << ", ";
        }
        std::cout << std::endl;*/
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
                std::cout << "Something might be wrong with the code..."<<std::endl;
            }
        }

        // if orders is full of invalid orders and events is empty, this will stop a nullptr dereference
        if(events.empty()) break;
        
        Event* e = *event;

        events.erase(event);

        if(e->getTimestamp() <= endTime && dynamic_cast<OutpostRangeEvent*>(e) && vessels.empty()) continue; 

        nextEndState = e->getTimestamp();

        if(e->getTimestamp() > endTime) {
            ranOutOfTime = true;
            break;
        }

        updateState(e->getTimestamp());

        e->run(this);
        simulatedEvents.push_back(e);

        updateEvents();
    }
    std::cout<<"no more events remain to be tracked."<<std::endl;
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
    case Mode::CONQUEST:
        std::sort(scores.begin(), scores.end(), [](Player* a, Player* b) {
            return a->getOutposts().size() != b->getOutposts().size() ? a->getOutposts().size() > b->getOutposts().size()
                : a->hasLost() != b->hasLost() ? !a->hasLost()
                : !a->hasLost() && a->getUnits() != b->getUnits() ? a->getUnits() > b->getUnits()
                : a->hasLost() && a->getDefeatedTime() != b->getDefeatedTime() ? a->getDefeatedTime() > b->getDefeatedTime()
                : a->getID() > b->getID();
        });
        
        break;
    }

    return scores;
}

bool Game::hasEnded() const {
    std::vector<Player*> sorted = sortedPlayers();

    // eliminating all other players always ends the game
    int alive = 0;
    for(const auto& pair : getPlayers()) {
        // only person with a queen is the winning player
        if(!pair.second->hasLost()) alive++;
    }
    if(alive <= 1) return true;

    switch(settings->gameMode) {
    case Mode::MINING:
        if(sorted.front()->getResources() < settings->resourcesToWin) return false;
        break;
    default: return false;
    }

    return true;
}

void Game::endGame() {
    endTime = stateTime;
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
    for(auto it = cache.begin(); it != cache.end(); it++) {
        if((*it)->getTime() > timestamp) break;
        else returnVal = *it;
    }

    if(returnVal->getTime() == getTime()) returnVal = shared_from_this();

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
    }

    for(Order* o : returnVal->getOrders()) {
        if(o->getID() == ID) {
            returnVal->removeOrder(o);
            break;
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


bool GameOrder::operator()(const std::shared_ptr<Game> &lhs, const std::shared_ptr<Game> &rhs) const {
    double diff = lhs->getTime() - rhs->getTime();
    return diff == 0 ? lhs->simulatedEventCount() - rhs->simulatedEventCount() < 0 : diff < 0;
}