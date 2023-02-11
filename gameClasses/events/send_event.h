#ifndef _SEND_EVENT_H_
#define _SEND_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../gameObjects/vessel.h"
#include "../gameObjects/outpost.h"
#include "../positional_object.h"
#include "../game.h"

class SendEvent : public Event
{
private:
    int numUnits;
    std::list<Specialist*> specialists;

    Player* owner;
    Outpost* outpost;
    PositionalObject* target;

public:
    SendEvent(){};
    SendEvent(time_t timestamp, int numUnits, const std::list<Specialist*> &specialists, Player* owner, Outpost* outpost, PositionalObject* target) :
        Event(timestamp), numUnits(numUnits), specialists(specialists), owner(owner), outpost(outpost), target(target) {}

    bool referencesObject(int id) const override { return false; }

    void run(Game* game) const override {
        Vessel vessel(owner, outpost->getPosition(), outpost, target, outpost->removeUnits(numUnits), outpost->removeSpecialists(specialists));

        game->addVessel(vessel);
    }
};

#endif