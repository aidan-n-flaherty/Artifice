#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <list>

class Specialist;

class Player
{
private:
    int id;

    std::string name;

    bool winner;

    std::list<Specialist*> specialists;

public:
    Player() {};
    Player(std::string name, int id) : id(id), name(name) {}

    double globalSpeed();

    bool hasWon() { return winner; }

    int attackPower(int numUnits);

    bool hasSpecialists(std::list<int> specialists);

    void hireSpecialist(Specialist* specialist) { specialists.push_back(specialist); }

    const int getID() const { return id; }
};

#endif