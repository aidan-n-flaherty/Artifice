#ifndef _REROUTE_EVENT_H_
#define _REROUTE_EVENT_H_

#include <cstdlib>
#include "../event.h"
#include "../game.h"

class RerouteEvent : public Event
{
private:
    Vessel* vessel;
    PositionalObject* target;
public:
    RerouteEvent(){};
    RerouteEvent(Order* o, double timestamp, Vessel* vessel, PositionalObject* target) :
        Event(o, timestamp), vessel(vessel), target(target) {}
    
    Event* copy() override { return new RerouteEvent(*this); }

    bool referencesObject(int id) const override { return vessel->getID() == id; }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        target = game->getPosObject(target->getID());
    }

    void run(Game* game) override {
        if(game->ignoreVessel(vessel->getID(), getTimestamp())) {
            setDisabled(true);
            return;
        }

        if(target->hasOwner() && vessel->hasOwner() && target->getOwnerID() != vessel->getOwnerID() && vessel->getTarget()->getOwnerID() != target->getOwnerID() && (!game->teamGame() || target->getOwner()->getTeamID() != vessel->getOwner()->getTeamID())) game->addNotification(new AttackNotification(getTimestamp(), vessel->getID(), target->getOwnerID()));

        vessel->setOriginatingOrder(getOriginatingOrder());
        vessel->setTarget(target);
        vessel->setOrigin(nullptr);
        vessel->setSpeedModifier(1.0);
        if(vessel->hasOwner()) vessel->getOwner()->addVesselPath(getTimestamp(), vessel->getPosition(), vessel->getTargetPos());

        for(auto& pair : game->getVessels()) {
            if(pair.second->getTargetID() == vessel->getID()) {
                pair.second->setOrigin(nullptr);
            }
        }
    }
};

#endif