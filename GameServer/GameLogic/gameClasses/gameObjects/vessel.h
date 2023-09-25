#ifndef _VESSEL_H_
#define _VESSEL_H_

#include <cstdlib>
#include <cmath>
#include <set>
#include "../helpers/point.h"
#include "positional_object.h"
#include "outpost.h"
#include "specialist.h"
#include "../event.h"
#include "../possessable.h"
#include "../gameObjects/player.h"

class Game;

class Vessel : public PositionalObject
{
private:
    Outpost* returnOutpost;
    Outpost* origin;
    PositionalObject* target;

    bool gift;

    double speedModifier = 1;

public:
    Vessel(){}
    Vessel(Player* owner, const Point& position, Outpost* origin, 
        PositionalObject* target, int numUnits,
        const std::list<Specialist*> &specialists) :
        PositionalObject(position, numUnits, specialists),
        returnOutpost(origin), origin(origin), target(target), gift(false) { owner->addVessel(this); }
    void updatePointers(Game* game) override;

    void collision(Vessel* vessel, Vessel* other, double timestamp, std::multiset<Event*, EventOrder> &events);
    void collision(Vessel* vessel, Outpost* other, double timestamp, std::multiset<Event*, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, Vessel* other);
    void specialistPhase(int& units, int& otherUnits, Outpost* other);

    Outpost* getOrigin() const { return origin; }
    int getOriginID() const { return origin != nullptr ? origin->getID() : -1; }
    void setOrigin(Outpost* origin) { this->origin = origin; }

    double getSpeed() const override;
    void setSpeedModifier(double speed) { this->speedModifier = speed; }

    bool isGift() const { return gift; }
    void setGift() { gift = true; } 

    PositionalObject* getTarget() const { return target; }
    const Point getTargetPos() const override;
    int getTargetID() const { return target->getID(); }
    const Point getPositionAt(double timeDiff) const override;

    void setTarget(PositionalObject* target) {
        setRefresh(this->target == nullptr || this->target->getID() != target->getID());
        this->target = target;
    }

    void returnHome();

    void update(double timeDiff);
};

#endif