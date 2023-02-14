#ifndef _REROUTE_EVENT_H_
#define _REROUTE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../game.h"

class RerouteEvent : public Event
{
private:
    std::shared_ptr<Vessel> vessel;
    std::shared_ptr<PositionalObject> target;
public:
    RerouteEvent(){};
    RerouteEvent(time_t timestamp, std::shared_ptr<Vessel> vessel, std::shared_ptr<PositionalObject> target) :
        Event(timestamp), vessel(vessel), target(target) {}
    RerouteEvent(const std::shared_ptr<RerouteEvent> other, Game* game);
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        target = game->getPosObject(target->getID());
    }

    void run(Game* game) const override {
        vessel->setTarget(target);
    }
};

#endif