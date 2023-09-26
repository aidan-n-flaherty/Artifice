#ifndef _PROMOTE_EVENT_H_
#define _PROMOTE_EVENT_H_

#include <cstdlib>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

class PromoteEvent : public Event
{
private:
    Specialist* specialist;

    SpecialistType promotion;

public:
    PromoteEvent(){};
    PromoteEvent(Order* o, double timestamp, Specialist* specialist, SpecialistType promotion) : Event(o, timestamp), specialist(specialist), promotion(promotion) {}
    
    Event* copy() override { return new PromoteEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        specialist->setOriginatingOrder(getOriginatingOrder());
        specialist->setType(promotion);
        specialist->getOwner()->removeHire();
    }
};

#endif