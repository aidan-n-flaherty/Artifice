#ifndef _POSITIONAL_OBJECT_H_
#define _POSITIONAL_OBJECT_H_

#include <cstdlib>
#include "game_object.h"
#include "helpers/point.h"
#include "gameObjects/specialist.h"

class PositionalObject : public GameObject
{
private:
    int numUnits;

    Point position;

    std::list<Specialist*> specialists;

public:
    PositionalObject(){}
    PositionalObject(Point position, int numUnits) : numUnits(numUnits), position(position) {}
    PositionalObject(Point position, int numUnits, std::list<Specialist*> specialists) : numUnits(numUnits), position(position), specialists(specialists) {}
    PositionalObject(double x, double y, int numUnits) : numUnits(numUnits), position(x, y) {}
    PositionalObject(double x, double y, int numUnits, std::list<Specialist*> specialists) : numUnits(numUnits), position(x, y), specialists(specialists) {}

    const Point& getPosition() const { return position; }
    virtual const Point getTargetPos(const Point& dimensions) const { return position; }
    double distance(const Point &dimensions, const Point &other) const;

    void moveTowards(const Point &dimensions, const Point &other, double distance);

    int getUnits() { return numUnits; }
    std::list<Specialist*> getSpecialists() { return specialists; }
    virtual double getSpeed() const { return 0; };

    void addUnits(int count) { numUnits += count; }
    void addSpecialists(std::list<Specialist*> specialists);

    bool hasSpecialist(SpecialistType t) const;

    bool canRemoveSpecialists(std::list<int> specialistIDs) const;
    bool canRemoveUnits(int count) const;

    // returns the number of units removed
    int removeUnits(int count);
    // returns the specialists that were removed
    std::list<Specialist*> removeSpecialists(std::list<Specialist*> specialists);
};

#endif