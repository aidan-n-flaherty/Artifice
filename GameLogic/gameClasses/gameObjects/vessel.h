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
    std::shared_ptr<Outpost> returnOutpost;
    std::shared_ptr<Outpost> origin;
    std::shared_ptr<PositionalObject> target;

    bool gift;

public:
    Vessel(){}
    Vessel(Player* owner, Point position, std::shared_ptr<Outpost> origin, 
        std::shared_ptr<PositionalObject> target, int numUnits,
        const std::list<std::shared_ptr<Specialist>> &specialists) :
        PositionalObject(position, numUnits, specialists), origin(origin),
        returnOutpost(origin), target(target), gift(false) { setOwner(owner); }
    void updatePointers(Game* game) override;

    void collision(std::shared_ptr<Vessel> vessel, std::shared_ptr<Vessel> other, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);
    void collision(std::shared_ptr<Vessel> vessel, std::shared_ptr<Outpost> other, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, std::shared_ptr<Vessel> other);
    void specialistPhase(int& units, int& otherUnits, std::shared_ptr<Outpost> other);

    std::shared_ptr<Outpost> getOrigin() const { return origin; }
    int getOriginID() const { return origin->getID(); }

    double getSpeed() const override;

    bool isGift() const { return gift; }
    void setGift() { gift = true; } 

    std::shared_ptr<PositionalObject> getTarget() const { return target; }
    const Point getTargetPos() const override;
    int getTargetID() const { return target->getID(); }
    const Point getPositionAt(double timeDiff) const override;

    void setTarget(std::shared_ptr<PositionalObject> target) {
        setRefresh(this->target == nullptr || this->target->getID() != target->getID());
        this->target = target;
    }

    void returnHome();

    void update(double timeDiff);
};

#endif