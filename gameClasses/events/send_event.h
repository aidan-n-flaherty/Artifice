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
    std::list<std::shared_ptr<Specialist>> specialists;

    std::shared_ptr<Player> owner;
    std::shared_ptr<Outpost> outpost;
    std::shared_ptr<PositionalObject> target;

public:
    SendEvent(){};
    SendEvent(time_t timestamp, int numUnits, const std::list<std::shared_ptr<Specialist>> &specialists, std::shared_ptr<Player> owner, std::shared_ptr<Outpost> outpost, std::shared_ptr<PositionalObject> target) :
        Event(timestamp), numUnits(numUnits), specialists(specialists), owner(owner), outpost(outpost), target(target) {}
    SendEvent(const std::shared_ptr<SendEvent> other, Game* game);
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        for(std::shared_ptr<Specialist> &a : this->specialists) a = game->getSpecialist(a->getID());
        owner = game->getPlayer(owner->getID());
        outpost = game->getOutpost(outpost->getID());
        target = game->getPosObject(target->getID());
    }

    void run(Game* game) const override {
        Vessel* vessel = new Vessel(owner, outpost->getPosition(), outpost, target, outpost->removeUnits(numUnits), outpost->removeSpecialists(specialists));

        game->addVessel(vessel);
    }
};

#endif