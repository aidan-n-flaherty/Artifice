#ifndef _REROUTE_ORDER_H_
#define _REROUTE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
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
    RerouteOrder(time_t timestamp, int senderID, int vesselID, int targetID, int referenceID) :
        Order(timestamp, senderID, referenceID), vesselID(vesselID), targetID(targetID) {}
    RerouteOrder(int id, time_t timestamp, int senderID, int vesselID, int targetID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), vesselID(vesselID), targetID(targetID) {}

    void adjustIDs(int createdID) override {
        if(vesselID >= createdID) vesselID++;
        if(targetID >= createdID) targetID++;
    }

    std::shared_ptr<Event> convert(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) return nullptr;
        
        if(!game->hasVessel(vesselID) || !game->hasPosObject(targetID)) return nullptr;

        std::shared_ptr<Vessel> vessel = game->getVessel(vesselID);
        std::shared_ptr<PositionalObject> target = game->getPosObject(targetID);

        if(vessel->getOwnerID() != getSenderID()) return nullptr;

        if(!vessel->controlsSpecialist(SpecialistType::NAVIGATOR)) return nullptr;
        if(game->hasVessel(targetID) && !vessel->controlsSpecialist(SpecialistType::PIRATE)) return nullptr;

        return std::shared_ptr<Event>(new RerouteEvent(getTimestamp(), vessel, target));
    }
};

#endif