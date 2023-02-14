#ifndef _OUTPOST_H_
#define _OUTPOST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include "../positional_object.h"
#include "../possessable.h"

enum OutpostType {
    GENERATOR,
    FACTORY,
    MINE
};

class Game;

class Outpost : public PositionalObject, public Possessable
{
private:
    double fractionalProduction;

    int maxShieldCharge;
    int shieldCharge;
    double fractionalShield;

public:
    Outpost(){}
    Outpost(int numUnits, double x, double y) : PositionalObject(x, y, numUnits), shieldCharge(0),
        maxShieldCharge(10), fractionalProduction(0), fractionalShield(0) {}

    int getShield(){ return shieldCharge; }

    void setMaxShield(int maxShieldCharge) { this->maxShieldCharge = maxShieldCharge; }

    int removeShield(int amount);

    int getUnitsAt(double timeDiff) override;

    int getShieldAt(double timeDiff);

    void update(double timeDiff);
};

#endif