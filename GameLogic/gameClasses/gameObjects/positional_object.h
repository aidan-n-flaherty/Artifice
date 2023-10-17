#ifndef _POSITIONAL_OBJECT_H_
#define _POSITIONAL_OBJECT_H_

#include <cstdlib>
#include <algorithm>
#include "../game_object.h"
#include "../helpers/point.h"
#include "specialist.h"
#include "../order.h"

class Game;

class GameSettings;

class PositionalObject : public GameObject, public Possessable
{
private:
    int numUnits = 0;

    Point position;

    std::list<Specialist*> specialists;

public:
    PositionalObject(unsigned int ID, GameSettings* settings, const Point &position, int numUnits) : GameObject(ID, settings), numUnits(numUnits), position(position) {}
    PositionalObject(unsigned int ID, GameSettings* settings, const Point &position, int numUnits, const std::list<Specialist*> &specialists) : GameObject(ID, settings), numUnits(numUnits), position(position) { addSpecialists(specialists); }
    PositionalObject(unsigned int ID, GameSettings* settings, double x, double y, int numUnits) : GameObject(ID, settings), numUnits(numUnits), position(settings, x, y) {}
    PositionalObject(unsigned int ID, GameSettings* settings, double x, double y, int numUnits, const std::list<Specialist*> &specialists) : GameObject(ID, settings), numUnits(numUnits), position(settings, x, y), specialists(specialists) { addSpecialists(specialists); }

    void updatePointers(Game* game) override;

    const Point& getPosition() const { return position; }
    virtual const Point getPositionAt(double timeDiff) const { return getPosition(); }
    virtual const Point getTargetPos() const { return getPosition(); }
    double distance(const Point& other) const;
    void moveTowards(const Point& other, double distance) { position.moveTowards(other, distance); };

    virtual int getUnitsAt(double timeDiff) const { return numUnits; }
    int getUnits() const { return numUnits; }

    std::list<Specialist*> getSpecialists() const { return specialists; }
    virtual double getSpeed() const { return 0; };

    void addUnits(int count) { numUnits += count; }
    void addSpecialist(Specialist* specialists);
    void addSpecialists(std::list<Specialist*> specialists);

    void setOwner(Player* player) override {
        Possessable::setOwner(player);
        setRefresh(true);
    }

    // just ignore setting to negative units
    int setUnits(int count) { numUnits = count >= 0 ? count : 0; return numUnits; }

    bool ownerControlsSpecialist(SpecialistType type) const;
    int ownerSpecialistCount(SpecialistType type) const;
    int specialistCount(SpecialistType t) const;
    bool controlsSpecialist(SpecialistType t) const;

    Specialist* getSpecialist(SpecialistType t);

    bool canRemoveSpecialists(std::list<int> specialistIDs) const;
    bool canRemoveUnits(int count) const;

    // returns the number of units removed
    int removeUnits(int count);
    // returns the specialists that were removed
    std::list<Specialist*> removeSpecialists(std::list<Specialist*> specialists);
    Specialist* removeSpecialist(Specialist* specialist);

    // removes and returns all specialists contained in this object
    std::list<Specialist*> removeSpecialists();
};

#endif