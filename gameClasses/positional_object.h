#ifndef _POSITIONAL_OBJECT_H_
#define _POSITIONAL_OBJECT_H_

#include <cstdlib>
#include "game_object.h"
#include "helpers/point.h"
#include "gameObjects/specialist.h"

class Game;

class PositionalObject : public GameObject
{
private:
    int numUnits;

    Point position;

    std::list<std::shared_ptr<Specialist>> specialists;

public:
    PositionalObject(){}
    PositionalObject(Point position, int numUnits) : numUnits(numUnits), position(position) {}
    PositionalObject(Point position, int numUnits, std::list<std::shared_ptr<Specialist>> specialists) : numUnits(numUnits), position(position), specialists(specialists) {}
    PositionalObject(double x, double y, int numUnits) : numUnits(numUnits), position(x, y) {}
    PositionalObject(double x, double y, int numUnits, std::list<std::shared_ptr<Specialist>> specialists) : numUnits(numUnits), position(x, y), specialists(specialists) {}
    PositionalObject(std::shared_ptr<PositionalObject> other, Game* game);
    void updatePointers(Game* game);

    const Point& getPosition() const { return position; }
    virtual const Point getPositionAt(const Point& dimensions, double timeDiff) const { return getPosition(); }
    virtual const Point getTargetPos(const Point& dimensions) const { return getPosition(); }
    double distance(const Point& dimensions, const Point& other) const;
    void moveTowards(const Point& dimensions, const Point& other, double distance) { position.moveTowards(dimensions, other, distance); };

    virtual int getUnitsAt(double timeDiff) const { return numUnits; }
    int getUnits() const { return numUnits; }

    std::list<std::shared_ptr<Specialist>> getSpecialists() const { return specialists; }
    virtual double getSpeed() const { return 0; };

    void addUnits(int count) { numUnits += count; }
    void addSpecialists(std::list<std::shared_ptr<Specialist>> specialists);

    bool hasSpecialist(SpecialistType t) const;

    bool canRemoveSpecialists(std::list<int> specialistIDs) const;
    bool canRemoveUnits(int count) const;

    // returns the number of units removed
    int removeUnits(int count);
    // returns the specialists that were removed
    std::list<std::shared_ptr<Specialist>> removeSpecialists(std::list<std::shared_ptr<Specialist>> specialists);
};

#endif