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
    std::shared_ptr<Vessel> vessel;

public:
    GiftEvent(){};
    GiftEvent(time_t timestamp, std::shared_ptr<Vessel> vessel) : Event(timestamp), vessel(vessel) {}
    
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
    }

    void run(Game* game) const override {
        vessel->setGift();
    }
};

#endif