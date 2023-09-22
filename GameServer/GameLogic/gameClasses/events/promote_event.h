#ifndef _PROMOTE_EVENT_H_
#define _PROMOTE_EVENT_H_

#include <cstdlib>
#include <cmath>
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
    PromoteEvent(double timestamp, Specialist* specialist, SpecialistType promotion) : Event(timestamp), specialist(specialist), promotion(promotion) {}
    
    Event* copy() override { return new PromoteEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        specialist->setType(promotion);
    }
};

#endif