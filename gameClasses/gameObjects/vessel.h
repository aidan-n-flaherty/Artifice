#ifndef _VESSEL_H_
#define _VESSEL_H_

#include <cstdlib>
#include <cmath>
#include <set>
#include "../helpers/point.h"
#include "../positional_object.h"
#include "outpost.h"
#include "specialist.h"
#include "../event.h"
#include "../possessable.h"

class Game;

class Vessel : public PositionalObject, public Possessable
{
private:
    Outpost* origin;
    PositionalObject* target;

public:
    Vessel(){};
    Vessel(Player* owner, Point position, Outpost* origin, PositionalObject* target, int numUnits, const std::list<Specialist*> &specialists) :
        PositionalObject(position, numUnits, specialists), Possessable(owner), origin(origin), target(target) {};

    void collision(const Point &dimensions, Vessel* other, time_t timestamp, std::multiset<Event*> &events);
    void collision(const Point &dimensions, Outpost* other, time_t timestamp, std::multiset<Event*> &events);

    Outpost* getOrigin() { return origin; }
    int getOriginID() const { return origin->getID(); }
    PositionalObject* getTarget() { return target; }
    const Point getTargetPos(const Point& dimensions) const override;
    double getSpeed() const override;

    int getTargetID() const { return target->getID(); }

    void setTarget(PositionalObject* target) { this->target = target; }

    void update(const Point &dimensions, double timeDiff);
};

#endif