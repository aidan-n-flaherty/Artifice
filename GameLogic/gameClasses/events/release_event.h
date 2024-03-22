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
    std::list<Specialist*> specialists;
    Outpost* outpost;

public:
    ReleaseEvent(){};
    ReleaseEvent(Order* o, double timestamp, std::list<Specialist*> specialists, Outpost* outpost) : Event(o, timestamp), specialists(specialists), outpost(outpost) {}
    ReleaseEvent(Order* o, double timestamp, Specialist* specialist, Outpost* outpost) : Event(o, timestamp), outpost(outpost) {specialists.push_front(specialist);}
    
    Event* copy() override { return new ReleaseEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        for(Specialist* &specialist:specialists){
            specialist = game->getSpecialist(specialist->getID());
        }
        outpost = game->getOutpost(outpost->getID());
    }

    void run(Game* game) override {
        //this'll need to be changed when multiple players own specialists from the same group of released specialists
        Player* owner = specialists.front()->getOwner();
        std::list<Outpost*> outposts = owner->sortedOutposts(outpost);
        for (Specialist*& specialist : specialists) {
            outpost->removeSpecialist(specialist);
        }
        Vessel* vessel = new Vessel(game->incrementObjCounter(), game->getSettings(), owner, outpost->getPosition(), outpost, outposts.front(), 0, specialists);
        vessel->setOriginatingOrder(getOriginatingOrder());

        game->addVessel(vessel);
    }
};

#endif