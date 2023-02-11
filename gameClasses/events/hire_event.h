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
    Player* owner;
    Specialist* specialist;

public:
    HireEvent(){};
    HireEvent(time_t timestamp, Player* owner, Specialist* specialist) : Event(timestamp), owner(owner), specialist(specialist) {}

    bool referencesObject(int id) const override { return false; }

    void run(Game* game) const override {
        owner->hireSpecialist(specialist);
    }
};

#endif