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
    Player* owner;
    Specialist* specialist;

public:
    HireEvent(){};
    HireEvent(double timestamp, Player* owner, Specialist* specialist) : Event(timestamp), owner(owner), specialist(specialist) {}
    
    Event* copy() override { return new HireEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        game->addSpecialist(specialist);
        owner->addSpecialist(specialist);
        owner->removeHire();

        PositionalObject* spawn = owner->getSpawnLocation();
        if(spawn) spawn->addSpecialist(specialist);       
    }
};

#endif