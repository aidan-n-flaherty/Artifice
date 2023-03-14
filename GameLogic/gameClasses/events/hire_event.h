#ifndef _HIRE_EVENT_H_
#define _HIRE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

class HireEvent : public Event
{
private:
    std::shared_ptr<Player> owner;
    std::shared_ptr<Specialist> specialist;

public:
    HireEvent(){};
    HireEvent(time_t timestamp, std::shared_ptr<Player> owner, std::shared_ptr<Specialist> specialist) : Event(timestamp), owner(owner), specialist(specialist) {}
    
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) const override {
        owner->addSpecialist(specialist);
    }
};

#endif