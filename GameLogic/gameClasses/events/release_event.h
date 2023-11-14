#ifndef _RELEASE_EVENT_H_
#define _RELEASE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <list>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

class ReleaseEvent : public Event
{
private:
    Specialist* specialist;

public:
    ReleaseEvent(){};
    ReleaseEvent(Order* o, double timestamp, Specialist* specialist) : Event(o, timestamp), specialist(specialist) {}
    
    Event* copy() override { return new ReleaseEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        std::list<Outpost*> outposts = specialist->getOwner()->sortedOutposts(specialist->getContainer());

        Vessel* vessel = new Vessel(game->incrementObjCounter(), game->getSettings(), specialist->getOwner(), specialist->getContainer()->getPosition(), specialist->getContainer(), outposts.front(), 0, std::list<Specialist*> { specialist->getContainer()->removeSpecialist(specialist) });
        vessel->setOriginatingOrder(getOriginatingOrder());

        game->addVessel(vessel);
    }
};

#endif