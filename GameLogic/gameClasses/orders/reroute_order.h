#ifndef _REROUTE_ORDER_H_
#define _REROUTE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../gameObjects/vessel.h"
#include "../event.h"
#include "../game.h"
#include "../events/reroute_event.h"

class RerouteOrder : public Order
{
private:
    int vesselID;
    int targetID;

public:
    RerouteOrder(){};
    RerouteOrder(double timestamp, int senderID, int vesselID, int targetID, int referenceID) :
        Order(timestamp, senderID, referenceID), vesselID(vesselID), targetID(targetID) {}
    RerouteOrder(int id, double timestamp, int senderID, int vesselID, int targetID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), vesselID(vesselID), targetID(targetID) {}

    Order* copy() override { return new RerouteOrder(*this); }

    void adjustIDs(int createdID, int amount) override {
        if(vesselID >= createdID) vesselID += amount;
        if(targetID >= createdID) targetID += amount;
    }

    Event* converted(Game* game) override {
        if(!game->hasPosObject(targetID)) return nullptr;

        PositionalObject* target = game->getPosObject(targetID);

        Outpost* targetOutpost = dynamic_cast<Outpost*>(target);

        if(targetOutpost) setDescription(std::string("Reroute submarine to " + targetOutpost->getName()));
        else setDescription(std::string("Reroute submarine to target an enemy submarine"));

        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;
        
        if(!game->hasVessel(vesselID)) return nullptr;

        if(targetID == vesselID) return nullptr;
        
        Vessel* vessel = game->getVessel(vesselID);
        

        if(vessel->getOwnerID() != getSenderID()) return nullptr;

        if(!vessel->controlsSpecialist(SpecialistType::NAVIGATOR)) return nullptr;
        if(game->hasVessel(targetID) && !vessel->controlsSpecialist(SpecialistType::PIRATE)) return nullptr;

        return new RerouteEvent(this, getTimestamp(), vessel, target);
    }

    std::string getType() const override { return "Reroute"; }
};

#endif