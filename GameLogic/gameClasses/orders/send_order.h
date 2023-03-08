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
    SendOrder(time_t timestamp, int senderID, int numUnits, std::list<int> specialistIDs, int originID, int targetID, int referenceID) :
        Order(timestamp, senderID, referenceID), numUnits(numUnits), specialistIDs(specialistIDs),
        originID(originID), targetID(targetID) {}

    void adjustIDs(int createdID) override {
        if(originID >= createdID) originID++;
        if(targetID >= createdID) targetID++;
        for(int& specialistID : specialistIDs) if(specialistID >= createdID) specialistID++;
    }

    std::shared_ptr<Event> convert(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;

        if(!game->hasOutpost(originID) || !game->hasPosObject(targetID)) return nullptr;

        std::shared_ptr<Outpost> outpost = game->getOutpost(originID);
        std::shared_ptr<PositionalObject> target = game->getPosObject(targetID);

        if(!outpost->canRemoveSpecialists(specialistIDs) || outpost->getOwnerID() != getSenderID() || !outpost->canRemoveUnits(numUnits)) return nullptr;

        std::list<std::shared_ptr<Specialist>> specialists;
        for(int specialistID : specialistIDs) specialists.push_back(game->getSpecialist(specialistID));

        updateOrders(game->getOrders());

        return std::shared_ptr<Event>(new SendEvent(getTimestamp(), numUnits, specialists, outpost, target));
    }
};

#endif