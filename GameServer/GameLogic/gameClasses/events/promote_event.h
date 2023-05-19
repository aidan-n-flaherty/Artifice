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
    Player* owner;
    Specialist* specialist;

    SpecialistType promotion;

public:
    PromoteEvent(){};
    PromoteEvent(time_t timestamp, Player* owner, Specialist* specialist, SpecialistType promotion) : Event(timestamp), owner(owner), specialist(specialist), promotion(promotion) {}
    
    Event* copy() override { return new PromoteEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) const override {
        specialist->setType(promotion);
    }
};

#endif