#ifndef _POSITIONAL_OBJECT_H_
#define _POSITIONAL_OBJECT_H_

#include <cstdlib>
#include "../game_object.h"
#include "../helpers/point.h"
#include "specialist.h"

class Game;

class PositionalObject : public GameObject, public Possessable
{
private:
    int numUnits;

    Point position;

    std::list<std::shared_ptr<Specialist>> specialists;

public:
    PositionalObject(){}
    PositionalObject(const Point &position, int numUnits) : numUnits(numUnits), position(position) {}
    PositionalObject(const Point &position, int numUnits, const std::list<std::shared_ptr<Specialist>> &specialists) : numUnits(numUnits), position(position), specialists(specialists) {}
    PositionalObject(double x, double y, int numUnits) : numUnits(numUnits), position(x, y) {}
    PositionalObject(double x, double y, int numUnits, const std::list<std::shared_ptr<Specialist>> &specialists) : numUnits(numUnits), position(x, y), specialists(specialists) {}
    PositionalObject(std::shared_ptr<PositionalObject> other, Game* game);

    void updatePointers(Game* game) override;

    const Point& getPosition() const { return position; }
    virtual const Point getPositionAt(double timeDiff) const { return getPosition(); }
    virtual const Point getTargetPos() const { return getPosition(); }
    double distance(const Point& other) const;
    void moveTowards(const Point& other, double distance) { position.moveTowards(other, distance); };

    virtual int getUnitsAt(double timeDiff) const { return numUnits; }
    int getUnits() const { return numUnits; }

    std::list<std::shared_ptr<Specialist>> getSpecialists() const { return specialists; }
    virtual double getSpeed() const { return 0; };

    void addUnits(int count) { numUnits += count; }
    void addSpecialist(std::shared_ptr<Specialist> specialists);
    void addSpecialists(std::list<std::shared_ptr<Specialist>> specialists);

    void setOwner(Player* player) override {
        Possessable::setOwner(player);
        setRefresh(true);
    }

    // just ignore setting to negative units
    int setUnits(int count) { numUnits = count >= 0 ? count : 0; return numUnits; }

    /* Note that int references are supplied in the function so that unit counts are only modified
    ** after phases on both sides are run, independent of order of execution.
    ** otherUnits should be a copy of the number of units the other positional object has.
    */
    void specialistPhase(int& unitDelta, int& otherUnitDelta, std::shared_ptr<PositionalObject> other);
    void postSpecialistPhase(int& unitDelta, int& otherUnitDelta, std::shared_ptr<PositionalObject> other);
    void victorySpecialistPhase(int unitDelta, int otherUnitDelta, std::shared_ptr<PositionalObject> other);
    void defeatSpecialistPhase(int unitDelta, int otherUnitDelta, std::shared_ptr<PositionalObject> other);
    void postCombatSpecialistPhase(Game* game, std::shared_ptr<PositionalObject> other);

    bool ownerControlsSpecialist(SpecialistType type) const;
    int ownerSpecialistCount(SpecialistType type) const;
    int specialistCount(SpecialistType t) const;
    bool controlsSpecialist(SpecialistType t) const;

    std::shared_ptr<Specialist> getSpecialist(SpecialistType t);

    bool canRemoveSpecialists(std::list<int> specialistIDs) const;
    bool canRemoveUnits(int count) const;

    // returns the number of units removed
    int removeUnits(int count);
    // returns the specialists that were removed
    std::list<std::shared_ptr<Specialist>> removeSpecialists(std::list<std::shared_ptr<Specialist>> specialists);
    std::shared_ptr<Specialist> removeSpecialist(std::shared_ptr<Specialist> specialist);

    // removes and returns all specialists contained in this object
    std::list<std::shared_ptr<Specialist>> removeSpecialists();
};

#endif