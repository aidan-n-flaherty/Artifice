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

class Player;

class Game;

class Vessel : public PositionalObject
{
private:
    Outpost* returnOutpost;
    Outpost* origin;
    PositionalObject* target;

    bool gift;

public:
    Vessel(){}
    Vessel(Player* owner, Point position, Outpost* origin, 
        PositionalObject* target, int numUnits,
        const std::list<Specialist*> &specialists) :
        PositionalObject(position, numUnits, specialists), origin(origin),
        returnOutpost(origin), target(target), gift(false) { setOwner(owner); }
    void updatePointers(Game* game) override;

    void collision(Vessel* vessel, Vessel* other, time_t timestamp, std::multiset<Event*, EventOrder> &events);
    void collision(Vessel* vessel, Outpost* other, time_t timestamp, std::multiset<Event*, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, Vessel* other);
    void specialistPhase(int& units, int& otherUnits, Outpost* other);

    Outpost* getOrigin() const { return origin; }
    int getOriginID() const { return origin->getID(); }

    double getSpeed() const override;

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