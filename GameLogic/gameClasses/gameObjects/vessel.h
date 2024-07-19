#ifndef _VESSEL_H_
#define _VESSEL_H_

#include <cstdlib>
#include <cmath>
#include <set>
#include <vector>
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

    Order* sourceOrder = nullptr;

    bool gift = false;

    double speedModifier = 1.0;

    bool disabled = false;

public:
    Vessel(unsigned int ID, GameSettings* settings, Player* owner, const Point& position, Outpost* origin, 
        PositionalObject* target, int numUnits,
        const std::list<Specialist*> &specialists) :
        PositionalObject(ID, settings, position, numUnits),
        returnOutpost(origin), origin(origin), target(target), gift(false) { if(owner) owner->addVessel(this); addSpecialists(specialists); }
    void updatePointers(Game* game) override;
    
    int getUnitsAt(double timeDiff) const override;

    void collision(Vessel* vessel, Vessel* other, double timestamp, std::multiset<Event*, EventOrder> &events, std::vector<Event*> &simulatedEvents);
    void collision(Vessel* vessel, Outpost* other, double timestamp, std::multiset<Event*, EventOrder> &events);

    void specialistPhase(int& units, int& otherUnits, Vessel* other);
    void specialistPhase(int& units, int& otherUnits, Outpost* other);

    Outpost* getOrigin() const { return origin; }
    int getOriginID() const { return origin != nullptr ? origin->getID() : -1; }
    void setOrigin(Outpost* origin) { this->origin = origin; }

    static double getSpeed(double speed, double simulationSpeed, Player* p, const std::list<Specialist*> &specialists, PositionalObject* source, PositionalObject* target, bool globalDisabled);
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

    PositionalObject* getReturnOutpost() { return returnOutpost; }

    void returnHome();

    void update(double timeDiff);
    
    void setSourceOrder(Order* o) { sourceOrder = o; }
    Order* getSourceOrder() { return sourceOrder; }

    void setDisabled(bool value) { this->disabled = value; }
    bool getDisabled() { return disabled; }

    int getProductionAmount() override;
};

#endif