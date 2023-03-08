#ifndef _SEND_EVENT_H_
#define _SEND_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../player.h"
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
    HireEvent(const std::shared_ptr<HireEvent> other, Game* game);
    
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) const override {
        owner->hireSpecialist(specialist);
    }
};

#endif