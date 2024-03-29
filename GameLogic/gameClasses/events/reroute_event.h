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
        vessel->setOriginatingOrder(getOriginatingOrder());
        vessel->setTarget(target);
        vessel->setOrigin(nullptr);
        vessel->setSpeedModifier(1.0);
    }
};

#endif