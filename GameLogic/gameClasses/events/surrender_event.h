#ifndef _SURRENDER_EVENT_H_
#define _SURRENDER_EVENT_H_

#include <cstdlib>
#include "../game.h"

class SurrenderEvent : public Event
{
private:
    Player* player;

public:
    SurrenderEvent(){};
    SurrenderEvent(Order* o, double timestamp, Player* player) :
        Event(o, timestamp), player(player) {}

    Event* copy() override { return new SurrenderEvent(*this); }

    bool referencesObject(int id) const override { return player->getID() == id; }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        player = game->getPlayer(player->getID());
    }

    void run(Game* game) override {
        player->setDefeated(game);
    }
};

#endif