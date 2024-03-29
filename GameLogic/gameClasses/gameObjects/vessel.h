#ifndef _VESSEL_H_
#define _VESSEL_H_

#include <cstdlib>
#include <cmath>
#include <set>
#include <algorithm>
#include "../helpers/point.h"
#include "positional_object.h"
#include "outpost.h"
#include "specialist.h"
#include "../event.h"
#include "../possessable.h"
#include "../gameObjects/player.h"

class Game;

class GameSettings;

class Vessel : public PositionalObject
{
private:
    Outpost* returnOutpost;
    Outpost* origin;
    PositionalObject* target;

    bool gift = false;

    double speedModifier = 1.0;

public:
    Vessel(unsigned int ID, GameSettings* settings, Player* owner, const Point& position, Outpost* origin, 
        PositionalObject* target, int numUnits,
        const std::list<Specialist*> &specialists) :
        PositionalObject(ID, settings, position, numUnits),
        returnOutpost(origin), origin(origin), target(target), gift(false) { owner->addVessel(this); addSpecialists(specialists); }
    void updatePointers(Game* game) override;

    void collision(Vessel* vessel, Vessel* other, double timestamp, std::multiset<Event*, EventOrder> &events);
    void collision(Vessel* vessel, Outpost* other, double timestamp, std::multiset<Event*, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, Vessel* other);
    void specialistPhase(int& units, int& otherUnits, Outpost* other);

    Outpost* getOrigin() const { return origin; }
    int getOriginID() const { return origin != nullptr ? origin->getID() : -1; }
    void setOrigin(Outpost* origin) { this->origin = origin; }

    static double getSpeed(double speed, double simulationSpeed, Player* p, std::list<Specialist*> specialists, PositionalObject* target);
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