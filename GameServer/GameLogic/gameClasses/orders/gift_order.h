#ifndef _GIFT_ORDER_H_
#define _GIFT_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../events/gift.h"

class GiftOrder : public Order
{
private:
    int vesselID;

public:
    GiftOrder(){};
    GiftOrder(time_t timestamp, int senderID, int vesselID, int referenceID) :
        Order(timestamp, senderID, referenceID), vesselID(vesselID) {}
    GiftOrder(int id, time_t timestamp, int senderID, int vesselID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), vesselID(vesselID) {}

    void adjustIDs(int createdID) override {
        if(vesselID >= createdID) vesselID++;
    }

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;
        
        if(!game->hasVessel(vesselID)) return nullptr;

        Vessel* vessel = game->getVessel(vesselID);

        if(vessel->getOwnerID() != getSenderID()) return nullptr;

        return new GiftEvent(getTimestamp(), vessel);
    }
};

#endif