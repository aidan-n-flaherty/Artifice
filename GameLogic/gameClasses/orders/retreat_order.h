#ifndef _RETREAT_ORDER_H_
#define _RETREAT_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../gameObjects/vessel.h"
#include "../event.h"
#include "../game.h"
#include "../events/retreat_event.h"

class RetreatOrder : public Order
{
private:
    int vesselID;

public:
    RetreatOrder(){};
    RetreatOrder(double timestamp, int senderID, int vesselID, int referenceID) :
        Order(timestamp, senderID, referenceID), vesselID(vesselID) {}
    RetreatOrder(int id, double timestamp, int senderID, int vesselID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), vesselID(vesselID) {}

    Order* copy() override { return new RetreatOrder(*this); }

    void adjustIDs(int createdID, int amount) override {
        if(vesselID >= createdID) vesselID += amount;
    }

    Event* converted(Game* game) override {
        setDescription(std::string("Order submarine to retreat"));

        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;
        
        if(!game->hasVessel(vesselID)) return nullptr;
        
        Vessel* vessel = game->getVessel(vesselID);

        if(vessel->getOwnerID() != getSenderID()) return nullptr;

        if(!game->canRetreat(vessel, 0)) return nullptr;

        return new RetreatEvent(this, getTimestamp(), vessel);
    }

    std::string getType() const override { return "Retreat"; }
};

#endif