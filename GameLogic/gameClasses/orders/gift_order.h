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
    GiftOrder(double timestamp, int senderID, int vesselID, int referenceID) :
        Order(timestamp, senderID, referenceID), vesselID(vesselID) {}
    GiftOrder(int id, double timestamp, int senderID, int vesselID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), vesselID(vesselID) {}
    
    Order* copy() override { return new GiftOrder(*this); }

    void adjustIDs(int createdID, int amount) override {
        if(vesselID >= createdID) vesselID += amount;
    }

    Event* converted(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;
        
        if(!game->hasVessel(vesselID)) return nullptr;

        Vessel* vessel = game->getVessel(vesselID);

        if(vessel->getOwnerID() != getSenderID()) return nullptr;

        setDescription(std::string("Convert vessel to gift"));

        return new GiftEvent(this, getTimestamp(), vessel);
    }

    std::string getType() const override { return "Gift"; }
};

#endif