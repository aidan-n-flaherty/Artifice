#ifndef _MINE_EVENT_H_
#define _MINE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"

class MineEvent : public Event
{
private:
    Outpost* outpost;

public:
    MineEvent(){};
    MineEvent(Order* o, double timestamp, Outpost* outpost) : Event(o, timestamp), outpost(outpost) {}
    
    Event* copy() override { return new MineEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        outpost = game->getOutpost(outpost->getID());
    }

    void run(Game* game) override {
        outpost->setType(OutpostType::MINE);
        outpost->setOriginatingOrder(getOriginatingOrder());
        outpost->removeUnits(game->getSettings()->costPerMine * (outpost->getOwner()->getMinesDrilled() + 1));
    }
};

#endif