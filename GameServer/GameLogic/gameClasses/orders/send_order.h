#ifndef _SEND_ORDER_H_
#define _SEND_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <set>
#include <iostream>
#include "../event.h"
#include "../game.h"
#include "../events/send_event.h"

class SendOrder : public Order
{
private:
    int numUnits;

    int originID;
    int targetID;
    std::list<int> specialistIDs;

public:
    SendOrder(){};
    SendOrder(double timestamp, int senderID, int numUnits, std::list<int> specialistIDs, int originID, int targetID, int referenceID) :
        Order(timestamp, senderID, referenceID), numUnits(numUnits), specialistIDs(specialistIDs),
        originID(originID), targetID(targetID) {}
    SendOrder(int id, double timestamp, int senderID, int numUnits, std::list<int> specialistIDs, int originID, int targetID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), numUnits(numUnits), specialistIDs(specialistIDs),
        originID(originID), targetID(targetID) {}

    void adjustIDs(int createdID) override {
        if(originID >= createdID) originID++;
        if(targetID >= createdID) targetID++;
        for(int& specialistID : specialistIDs) if(specialistID >= createdID) specialistID++;
    }

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;

        if(!game->hasOutpost(originID) || !game->hasPosObject(targetID)) return nullptr;

        if(originID == targetID) return nullptr;

        Outpost* outpost = game->getOutpost(originID);
        PositionalObject* target = game->getPosObject(targetID);

        if(!outpost->canRemoveSpecialists(specialistIDs) || outpost->getOwnerID() != getSenderID() || !outpost->canRemoveUnits(numUnits)) return nullptr;

        std::list<Specialist*> specialists;
        for(int specialistID : specialistIDs) specialists.push_back(game->getSpecialist(specialistID));

        // only pirates may target vessels
        if(game->hasVessel(targetID)) {
            bool hasPirate = false;

            for(Specialist* specialist : specialists) {
                if(specialist->getType() == SpecialistType::PIRATE) {
                    hasPirate = true;
                    break;
                }
            }

            if(!hasPirate) return nullptr;
        }

        updateOrders(game->getOrders());

        return new SendEvent(this, getTimestamp(), numUnits, specialists, outpost, target);
    }
};

#endif