#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <list>

enum class SpecialistType;

class Specialist;

class Game;

class Player
{
private:
    int id;

    std::string name;

    bool victor;

    std::list<std::shared_ptr<Specialist>> specialists;

    int resources;

public:
    Player() {};
    Player(std::string name, int id) : id(id), name(name), victor(false), resources(0) {}
    void updatePointers(Game* game);

    double globalSpeed();

    bool hasWon() { return victor; }

    int attackPower(int numUnits);

    bool hasSpecialist(SpecialistType t) const;

    bool hasSpecialists(std::list<int> specialists);

    void hireSpecialist(std::shared_ptr<Specialist> specialist) { specialists.push_back(specialist); }

    int getID() const { return id; }

    const std::string& getName() const { return name; }

    int getResources() { return resources; }

    void removeResources(int amount) { resources -= amount; }
};

#endif