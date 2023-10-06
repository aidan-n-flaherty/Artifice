#ifndef _SEND_EVENT_H_
#define _SEND_EVENT_H_

#include <cstdlib>
#include "../gameObjects/vessel.h"
#include "../gameObjects/outpost.h"
#include "../gameObjects/positional_object.h"
#include "../game.h"

class SendEvent : public Event
{
private:
    int numUnits;
    std::list<Specialist*> specialists;

    Outpost* outpost;
    PositionalObject* target;

public:
    SendEvent(){};
    SendEvent(Order* o, double timestamp, int numUnits, const std::list<Specialist*> &specialists, Outpost* outpost, PositionalObject* target) :
        Event(o, timestamp), numUnits(numUnits), specialists(specialists), outpost(outpost), target(target) {}

    Event* copy() override { return new SendEvent(*this); }

    bool referencesObject(int id) const override { return target->getID() == id; }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        for(Specialist* &a : this->specialists) a = game->getSpecialist(a->getID());
        outpost = game->getOutpost(outpost->getID());
        target = game->getPosObject(target->getID());
    }

    void run(Game* game) override {
        Vessel* vessel = new Vessel(game->incrementObjCounter(), game->getSettings(), outpost->getOwner(), outpost->getPosition(), outpost, target, outpost->removeUnits(numUnits), outpost->removeSpecialists(specialists));
        vessel->setOriginatingOrder(getOriginatingOrder());

        game->addVessel(vessel);
    }
};

#endif