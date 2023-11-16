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

    Outpost* outpost;

public:
    ReleaseEvent(){};
    ReleaseEvent(Order* o, double timestamp, Specialist* specialist, Outpost* outpost) : Event(o, timestamp), specialist(specialist), outpost(outpost) {}
    
    Event* copy() override { return new ReleaseEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        specialist = game->getSpecialist(specialist->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    void run(Game* game) override {
        std::list<Outpost*> outposts = specialist->getOwner()->sortedOutposts(outpost);

        std::list<Specialist*> specialists;
        specialists.push_back(outpost->removeSpecialist(specialist));
        Vessel* vessel = new Vessel(game->incrementObjCounter(), game->getSettings(), specialist->getOwner(), outpost->getPosition(), outpost, outposts.front(), 0, specialists);
        vessel->setOriginatingOrder(getOriginatingOrder());

        game->addVessel(vessel);
    }
};

#endif