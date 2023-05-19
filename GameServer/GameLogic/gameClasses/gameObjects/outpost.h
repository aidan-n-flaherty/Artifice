#ifndef _OUTPOST_H_
#define _OUTPOST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include "positional_object.h"
#include "../possessable.h"

enum OutpostType {
    GENERATOR,
    FACTORY,
    MINE,
    BROKEN
};

class Game;

class Vessel;

class Outpost : public PositionalObject
{
private:
    double fractionalProduction;

    int maxShieldCharge;
    int shieldCharge;
    double fractionalShield;

    OutpostType type;

public:
    Outpost(){}
    Outpost(OutpostType type, int numUnits, double x, double y) : PositionalObject(x, y, numUnits), shieldCharge(0),
        maxShieldCharge(10), fractionalProduction(0), fractionalShield(0), type(type) {}

    OutpostType getType() const { return type; }
    void setType(OutpostType type) { this->type = type; }

    void specialistPhase(int& units, int& otherUnits, Vessel* other);

    void setMaxShield(int maxShieldCharge) { this->maxShieldCharge = maxShieldCharge; }
    int removeShield(int amount);

    int getFireRange() const;
    int getSonarRange() const;
    int getShield() const { return shieldCharge; };
    int getMaxShield() const;
    int getUnitsAt(double timeDiff) const override;
    int getShieldAt(double timeDiff) const;
    int getUnitsAt(double& fractionalProduction, double timeDiff) const;
    int getShieldAt(double& fractionalShield, double timeDiff) const;

    void update(double timeDiff);
};

#endif