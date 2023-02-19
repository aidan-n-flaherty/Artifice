#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <list>

enum class SpecialistType;

class Specialist;

class Outpost;

class Vessel;

class Game;

class Player
{
private:
    int id;

    std::string name;

    bool victor;
    bool defeated;

    std::list<std::shared_ptr<Specialist>> specialists;
    std::list<std::shared_ptr<Outpost>> outposts;
    std::list<std::shared_ptr<Vessel>> vessels;

    int resources;
    double fractionalProduction;

public:
    Player() {}
    Player(std::string name, int id) : id(id), name(name), victor(false), defeated(false), resources(0), fractionalProduction(0) {}
    
    void updatePointers(Game* game);

    void update(double timeDiff);

    double globalSpeed();
    int globalMaxShield();
    int globalProductionAmount();
    double globalProductionSpeed();
    double globalSonar();

    bool hasWon() const { return victor; }
    bool hasLost() const { return defeated; }

    void setVictor() { victor = true; }
    void setDefeated() { defeated = true; }

    int specialistCount(SpecialistType t) const;
    bool hasSpecialist(SpecialistType t) const;
    bool hasSpecialists(std::list<int> specialists);

    std::list<std::shared_ptr<Specialist>> getSpecialists() const { return specialists; }
    void addSpecialist(std::shared_ptr<Specialist> specialist);
    void removeSpecialist(std::shared_ptr<Specialist> specialist);

    std::list<std::shared_ptr<Outpost>> getOutposts() const { return outposts; }
    void addOutpost(std::shared_ptr<Outpost> outpost) { outposts.push_back(outpost); }
    void removeOutpost(std::shared_ptr<Outpost> outpost) { outposts.remove(outpost); }

    std::list<std::shared_ptr<Vessel>> getVessels() const { return vessels; }
    void addVessel(std::shared_ptr<Vessel> vessel) { vessels.push_back(vessel); }
    void removeVessel(std::shared_ptr<Vessel> vessel) { vessels.remove(vessel); }

    int getID() const { return id; }
    const std::string& getName() const { return name; }

    int getResources() const { return resources; }
    int getCapacity() const;
    int getUnits() const;

    void addResources(int amount) { resources += amount; }
    void removeResources(int amount) { resources -= amount; }
};

#endif