#ifndef _GIFT_EVENT_H_
#define _GIFT_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

class GiftEvent : public Event
{
private:
    Vessel* vessel;

public:
    GiftEvent(){};
    GiftEvent(double timestamp, Vessel* vessel) : Event(timestamp), vessel(vessel) {}
    
    Event* copy() override { return new GiftEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
    }

    void run(Game* game) override {
        vessel->setGift();
    }
};

#endif