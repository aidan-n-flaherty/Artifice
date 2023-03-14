#ifndef _WIN_CONDITION_EVENT_H_
#define _WIN_CONDITION_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

/* This event should be scheduled whenever there is a possibility of a player losing
** or a player winning.
*/
class WinConditionEvent : public Event
{
private:
    std::shared_ptr<Player> player;

public:
    WinConditionEvent(){};
    WinConditionEvent(time_t timestamp, std::shared_ptr<Player> player) : Event(timestamp), player(player) {}
    
    bool referencesObject(int id) const override { return player->getID() == id; }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        player = game->getPlayer(player->getID());
    }

    void run(Game* game) const override {
        if(game->hasEnded()) game->endGame();
    }
};

#endif