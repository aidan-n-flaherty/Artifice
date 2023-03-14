#include <cstdlib>
#include <iostream>
#include <ctime>
#include <set>
#include <map>
#include <vector>
#include "game.h"
#include "gameObjects/vessel.h"
#include "order.h"
#include "helpers/point.h"
#include "events/send_event.h"
#include "events/reroute_event.h"
#include "events/outpost_range_event.h"
#include "game_settings.h"

int Game::width = 0;
int Game::height = 0;

/* The game constructor should create the entire starting state deterministically based on
** the random seed provided.
*/
Game::Game(time_t startTime, time_t endTime, std::map<int, std::string> &playerInfo, int seed, bool cacheEnabled) :
    stateTime(startTime), endTime(endTime), cacheEnabled(cacheEnabled) {
    std::srand(seed);

    width = 100000;
    height = 100000;

    for(auto pair : playerInfo) addPlayer(new Player(pair.second, pair.first, 1000));

    int t = 0;
    for(auto pair : playerInfo) {
        Outpost* o = new Outpost(OutpostType::FACTORY, 20, 10000 + 10000 * (t % 2), 10000 + 10000 * (t % 3) * (t % 3));
        addOutpost(o);
        getPlayer(t)->addOutpost(getOutpost(o->getID()));

        Specialist* s = new Specialist(t == 1 ? SpecialistType::PIRATE : SpecialistType::INFILTRATOR);
        addSpecialist(s);
        o->addSpecialist(getSpecialist(s->getID()));
        getPlayer(t)->addSpecialist(getSpecialist(s->getID()));

        t++;
        std::cout << pair.second << " has an outpost at (" << o->getPosition().getX() << ", " << o->getPosition().getY() << ")" << std::endl;
    }

    t = 0;
    for(auto pair : playerInfo) {
        Specialist* s = new Specialist(SpecialistType::QUEEN);
        addSpecialist(s);
        getPlayer(t)->getOutposts().front()->addSpecialist(getSpecialist(s->getID()));
        getPlayer(t)->addSpecialist(getSpecialist(s->getID()));
        t++;
    }
}

Game::Game(const Game& game) : stateTime(game.stateTime), cacheEnabled(game.cacheEnabled) {
    for(const std::shared_ptr<Event>& event : game.events) events.insert(std::shared_ptr<Event>(new Event(*event)));
    for(const auto& pair : game.vessels) vessels[pair.first] = std::shared_ptr<Vessel>(new Vessel(*pair.second));
    for(const auto& pair : game.players) players[pair.first] = std::shared_ptr<Player>(new Player(*pair.second));
    for(const auto& pair : game.outposts) outposts[pair.first] = std::shared_ptr<Outpost>(new Outpost(*pair.second));
    for(const auto& pair : game.specialists) specialists[pair.first] = std::shared_ptr<Specialist>(new Specialist(*pair.second));

    for(const std::shared_ptr<Event>& event : events) event->updatePointers(this);
    for(const auto& pair : vessels) pair.second->updatePointers(this);
    for(const auto& pair : players) pair.second->updatePointers(this);
    for(const auto& pair : outposts) pair.second->updatePointers(this);
    for(const auto& pair : specialists) pair.second->updatePointers(this);
    for(const auto& pair : players) pair.second->updatePointers(this);

    orders = game.orders;
    invalidOrders = game.invalidOrders;

    // note that even though the cache is full of pointers, there's no need for a deep copy
    // since whenever we modify a game state it leaves the cache
    cache = game.cache;
}

void Game::removeRelevant(int id) {
    for (auto it = events.begin(); it != events.end();) {
        if((*it)->referencesObject(id)) events.erase(it++);
        else ++it;
    }
}

void Game::updateEvents() {
    // All players flagged for update (e.g. increased mining rate)
    for(auto itA = players.begin(); itA != players.end(); itA++) {
        std::shared_ptr<Player>& player = itA->second;

        if(!player->needsRefresh()) continue;

        removeRelevant(player->getID());

        player->projectedVictory(player, stateTime, events);
    }

    // All outposts flagged for update (e.g. changed specialists)
    for(auto itA = outposts.begin(); itA != outposts.end(); itA++) {
        std::shared_ptr<Outpost>& outpost = itA->second;

        if(!outpost->needsRefresh()) continue;

        removeRelevant(outpost->getID());

        if(outpost->controlsSpecialist(SpecialistType::HYPNOTIST)) {
            for(const std::shared_ptr<Specialist> &s : outpost->getSpecialists()) {
                s->setOwner(outpost->getOwner());
            }
        }

        if(outpost->controlsSpecialist(SpecialistType::SENTRY)) {
            addEvent(new OutpostRangeEvent(getTime(), outpost));
        }
    }

    // All vessels flagged for update (e.g. global speed change) may have different combat times.
    for(auto itA = vessels.begin(); itA != vessels.end(); itA++) {
        std::shared_ptr<Vessel>& vessel = itA->second;

        if(!vessel->needsRefresh()) continue;

        removeRelevant(vessel->getID());

        for(auto itB = vessels.begin(); itB != vessels.end(); itB++) {
            if(itA == itB) continue;
            const std::shared_ptr<Vessel>& otherVessel = itB->second;
            
            vessel->collision(Point(getWidth(), getHeight()), vessel, otherVessel, stateTime, events);
        }

        for(auto itB = outposts.begin(); itB != outposts.end(); itB++) {
            const std::shared_ptr<Outpost>& outpost = itB->second;

            vessel->collision(Point(getWidth(), getHeight()), vessel, outpost, stateTime, events);
        }

        vessel->setRefresh(false);
    }

    cacheState();
}

void Game::updateState(time_t timestamp) {
    double secondsElapsed = difftime(timestamp, stateTime);

    for(auto& pair : vessels) pair.second->update(Point(getWidth(), getHeight()), secondsElapsed);
    for(auto& pair : outposts) pair.second->update(secondsElapsed);
    
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
    events.clear();
    updateEvents();

    // loops until no events or orders remain.
    // note that events will always be run before orders given the same timestamp.
    while(!events.empty() || !orders.empty()) {
        std::multiset<std::shared_ptr<Event>>::iterator event = events.begin();

        /*for(auto it = events.begin(); it != events.end(); it++) {
            std::cout << (*it)->getTimestamp() << ", ";
        }
        std::cout << std::endl;*/
        // check if the next chronological event occurs after the next chronological order, and if so,
        // run the order instead of the event
        while(!orders.empty() && (events.empty() || (*event)->getTimestamp() > (*orders.begin())->getTimestamp())) {
            std::multiset<std::shared_ptr<Order>>::iterator orderIt = orders.begin();
            std::shared_ptr<Order> order = *orderIt;
            // need an updated state to check for order validity
            updateState(order->getTimestamp());

            // converts a order to an event
            std::shared_ptr<Event> converted = order->convert(this);
            orders.erase(orderIt);

            if(!converted) {
                invalidOrders.push_back(order);
            } else {
                event = events.insert(converted);
                break;
            }
        }

        // if orders is full of invalid orders and events is empty, this will stop a nullptr dereference
        if(events.empty()) break;
        
        std::shared_ptr<Event> e = *event;
        events.erase(event);

        // only simulate the game until a certain time so that infinite future simulation does not happen
        if(e->getTimestamp() > endTime) break;

        updateState(e->getTimestamp());
        
        e->run(this);
        simulatedEvents.push_back(e);

        updateEvents();
    }

    return getScores();
}

std::vector<std::shared_ptr<Player>> Game::sortedPlayers() const {
    std::vector<std::shared_ptr<Player>> scores;
    for(auto& pair : getPlayers()) scores.push_back(pair.second);

    switch(GameSettings::gameMode) {
    case Mode::MINING:
        std::sort(scores.begin(), scores.end(), [](const std::shared_ptr<Player>& a, const std::shared_ptr<Player>& b) {
            return a->getResources() != b->getResources() ? a->getResources() > b->getResources()
                : a->hasLost() != b->hasLost() ? !a->hasLost()
                : !a->hasLost() && a->getUnits() != b->getUnits() ? a->getUnits() > b->getUnits()
                : a->hasLost() && a->getDefeatedTime() != b->getDefeatedTime() ? a->getDefeatedTime() > b->getDefeatedTime()
                : a->getID() > b->getID();
        });

        break;
    case Mode::CONQUEST:
        std::sort(scores.begin(), scores.end(), [](const std::shared_ptr<Player>& a, const std::shared_ptr<Player>& b) {
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
    std::vector<std::shared_ptr<Player>> sorted = sortedPlayers();

    // eliminating all other players always ends the game
    int alive = 0;
    for(const auto& pair : getPlayers()) {
        // only person with a queen is the winning player
        if(!pair.second->hasLost()) alive++;
    }
    if(alive <= 1) return true;

    switch(GameSettings::gameMode) {
    case Mode::MINING:
        if(sorted.front()->getResources() < GameSettings::resourcesToWin) return false;
        break;
    default: break;
    }

    return true;
}

void Game::endGame() {
    endTime = stateTime;
}

std::list<std::pair<int, int>> Game::getScores() {
    std::list<std::pair<int, int>> scores;

    std::vector<std::shared_ptr<Player>> sorted = sortedPlayers();

    if(!hasEnded()) return scores;

    for(int i = 0; i < sorted.size(); i++) {
        // 1st place has a score of 1, last place has a score of 0, linear interpolation for all in between
        double score = (sorted.size() - 1 - i) / (sorted.size() - 1);
        double scoreDelta = 0;

        for(int j = 0; j < sorted.size(); j++) {
            if(i == j) continue;

            double expected = 1.0 / (1 + pow(10, (sorted[j]->getRating() - sorted[i]->getRating())/400.0));
            scoreDelta += GameSettings::eloKValue * (score - expected);
        }

        scores.push_back(std::make_pair<int, int>(sorted[i]->getUserID(), round(scoreDelta / (sorted.size() - 1))));
    }

    return scores;
}

std::shared_ptr<Game> Game::lastState(time_t timestamp) const {
    std::shared_ptr<Game> returnVal = nullptr;

    for(auto it = cache.begin(); it != cache.end(); it++) {
        if((*it)->getTime() > timestamp) break;
        else returnVal = *it;
    }

    return returnVal;
}

time_t Game::nextState(time_t timestamp) {
    time_t returnTime = timestamp;

    for(auto it = cache.begin(); it != cache.end(); it++) {
        returnTime = (*it)->getTime();
        if((*it)->getTime() > timestamp) break;
    }

    return returnTime;
}

const std::shared_ptr<Event> Game::nextAssociatedEvent(time_t timestamp, int id) {
    for(const std::shared_ptr<Event>& event : simulatedEvents) {
        if(event->getTimestamp() > timestamp && event->referencesObject(id)) return event;
    }

    return nullptr;
}

void Game::addPlayer(Player* p) {
    players[p->getID()] = std::shared_ptr<Player>(p);
}

void Game::addVessel(Vessel* v) {
    vessels[v->getID()] = std::shared_ptr<Vessel>(v);
}

void Game::addOutpost(Outpost* o) {
    outposts[o->getID()] = std::shared_ptr<Outpost>(o);
}

void Game::addSpecialist(Specialist* s) {
    specialists[s->getID()] = std::shared_ptr<Specialist>(s);
}

void Game::addOrder(Order* o) {
    orders.insert(std::shared_ptr<Order>(o));
}

void Game::addEvent(Event* e) {
    events.insert(std::shared_ptr<Event>(e));
}

void Game::removeVessel(std::shared_ptr<Vessel> v) {
    removeRelevant(v->getID());
    if(v->hasOwner()) v->getOwner()->removeVessel(v);
    while(!v->getSpecialists().empty()) removeSpecialist(v->getSpecialists().front());
    v->remove();
    
    vessels.erase(v->getID());
}

void Game::removeSpecialist(std::shared_ptr<Specialist> s) {
    if(s->hasOwner()) s->getOwner()->removeSpecialist(s);
    if(s->getContainer() != nullptr) s->getContainer()->removeSpecialist(s);
    s->remove();

    specialists.erase(s->getID());
}

bool GameOrder::operator()(const std::shared_ptr<Game> &lhs, const std::shared_ptr<Game> &rhs) const {
    return difftime(lhs->getTime(), rhs->getTime()) < 0;
}