#include <cstdlib>
#include <iostream>
#include <ctime>
#include <set>
#include "game.h"
#include "gameObjects/vessel.h"
#include "order.h"
#include "helpers/point.h"
#include "events/send_event.h"
#include "events/invalid_event.h"

Game::Game(time_t startTime, std::list<Player> &players, int seed) : idCounter(0), stateTime(startTime) {
    width = 100000;
    height = 100000;

    int tmp = 10000;
    for(Player &p : players) {
        addPlayer(p);

        Outpost o(20, tmp, tmp);
        o.setOwner(&p);
        addOutpost(o);

        tmp += 12000;
    }
}

/* IMPORTANT: All order validity preprocessing occurs in this function.
** It should be expected that no error checking occurs after the order is
** processed and enters the event queue.
*/
Event* Game::processOrder(Order &order) {
    if(order.getTimestamp() < stateTime) return new InvalidEvent(order.getTimestamp());

    updateState(order.getTimestamp());

    std::list<int>& params = order.getParams();
    int ownerID = order.getSenderID();

    switch(order.getType()) {
        case SEND: {
            if(params.size() < 3) break;

            int originID = params.front(); params.pop_front();
            int targetID = params.front(); params.pop_front();
            int numUnits = params.front(); params.pop_front();
            
            std::list<int> specialistIDs;
            while(!params.empty()) {
                specialistIDs.push_back(params.front());
                params.pop_front();
            }
            
            if(!hasPlayer(ownerID) || !hasOutpost(originID) || !hasPosObject(targetID)) break;

            Player* owner = &getPlayer(ownerID);
            Outpost* outpost = &getOutpost(originID);
            PositionalObject* target = &getPosObject(targetID);

            if(!outpost->canRemoveSpecialists(specialistIDs) || outpost->getOwnerID() != ownerID || !outpost->canRemoveUnits(numUnits)) break;

            std::list<Specialist*> specialists;
            for(int specialistID : specialistIDs) specialists.push_back(&getSpecialist(specialistID));

            return new SendEvent(order.getTimestamp(), numUnits, specialists, owner, outpost, target);
        } case HIRE: {
            break;
        } case PROMOTE: {
            break;
        } default: {
            break;
        }
    }

    return new InvalidEvent(order.getTimestamp());
}

void Game::removeRelevant(int id) {
    for (auto it = events.begin(); it != events.end();) {
        if((*it)->referencesObject(id)) events.erase(it++);
        else ++it;
    }
}

void Game::updateEvents() {
    // All vessels flagged for update (e.g. global speed change) may have different combat times.
    for(auto itA = vessels.begin(); itA != vessels.end(); itA++) {
        Vessel &vessel = itA->second;

        if(!vessel.needsRefresh()) continue;

        removeRelevant(vessel.getID());

        for(auto itB = std::next(itA); itB != vessels.end(); itB++) {
            Vessel &otherVessel = itB->second;

            vessel.collision(Point(getWidth(), getHeight()), &otherVessel, stateTime, events);
        }

        for(auto itB = outposts.begin(); itB != outposts.end(); itB++) {
            Outpost &outpost = itB->second;

            vessel.collision(Point(getWidth(), getHeight()), &outpost, stateTime, events);
        }

        vessel.setRefresh(false);
    }
}

void Game::updateState(time_t timestamp) {
    double secondsElapsed = difftime(timestamp, stateTime);

    for(auto &pair : vessels) pair.second.update(Point(getWidth(), getHeight()), secondsElapsed);
    for(auto &pair : outposts) pair.second.update(secondsElapsed);
    
    stateTime = timestamp;
}

/* Only run this function when the game has finished.
** Returns a list of player IDs, ordered by their final score.
** Returns an empty list if the game is still in progress.
** Fills up the inputted list of invalid orders with orders that were rejected.
*/
std::list<int> Game::run(std::multiset<Order> &orders, std::list<Order> &invalid) {
    events.clear();

    updateEvents();

    // loops until no events or orders remain.
    // note that events will always be run before orders given the same timestamp.
    while(!events.empty() || !orders.empty()) {
        std::multiset<Event*>::iterator event = events.begin();

        // check if the next chronological event occurs after the next chronological order, and if so,
        // run the order instead of the event
        while(events.empty() || (!orders.empty() && (*event)->getTimestamp() > orders.begin()->getTimestamp())) {
            std::multiset<Order>::iterator orderIt = orders.begin();
            Order order = *orderIt;

            // need an updated state to check for order validity
            updateState(order.getTimestamp());

            // converts a order to an event
            Event* converted = processOrder(order);
            orders.erase(orderIt);

            if(!converted->valid()) {
                invalid.push_back(order);
            } else {
                event = events.insert(converted);
                break;
            }
        }

        // if orders is full of invalid orders and events is empty, this will stop a nullptr dereference
        if(events.empty()) break;
        Event* e = *event;
        events.erase(event);

        updateState(e->getTimestamp());
        
        e->run(this);
        delete e;

        updateEvents();
    }

    std::list<int> scores;
    return scores;
}

void Game::addPlayer(Player &p) {
    players[p.getID()] = p;
}

void Game::addVessel(Vessel &v) {
    v.setID(idCounter++);

    vessels[v.getID()] = v;
}

void Game::addOutpost(Outpost &o) {
    o.setID(idCounter++);

    outposts[o.getID()] = o;
}

void Game::removeVessel(Vessel &v) {
    v.remove();
    vessels.erase(v.getID());

    removeRelevant(v.getID());
}