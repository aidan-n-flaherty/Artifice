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

class Game;

class Vessel : public PositionalObject
{
private:
    std::shared_ptr<Outpost> origin;
    std::shared_ptr<PositionalObject> target;

public:
    Vessel(){}
    Vessel(std::shared_ptr<Player> owner, Point position, std::shared_ptr<Outpost> origin, 
        std::shared_ptr<PositionalObject> target, int numUnits,
        const std::list<std::shared_ptr<Specialist>> &specialists) :
        PositionalObject(position, numUnits, specialists), origin(origin), target(target) { setOwner(owner); }
    void updatePointers(Game* game) override;

    void collision(const Point &dimensions, std::shared_ptr<Vessel> vessel, std::shared_ptr<Vessel> other, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);
    void collision(const Point &dimensions, std::shared_ptr<Vessel> vessel, std::shared_ptr<Outpost> other, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, std::shared_ptr<Vessel> other);
    void specialistPhase(int& units, int& otherUnits, std::shared_ptr<Outpost> other);

    std::shared_ptr<Outpost> getOrigin() const { return origin; }
    int getOriginID() const { return origin->getID(); }

    double getSpeed() const override;

    std::shared_ptr<PositionalObject> getTarget() const { return target; }
    const Point getTargetPos(const Point& dimensions) const override;
    int getTargetID() const { return target->getID(); }

    const Point getPositionAt(const Point& dimensions, double timeDiff) const override;

    void setTarget(std::shared_ptr<PositionalObject> target) { setRefresh(true); this->target = target; }

    void update(const Point &dimensions, double timeDiff);
};

#endif