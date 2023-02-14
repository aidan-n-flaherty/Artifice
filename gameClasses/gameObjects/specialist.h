#ifndef _SPECIALIST_H_
#define _SPECIALIST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include "../game_object.h"
#include "../possessable.h"

enum class SpecialistType {
    QUEEN,
    PRINCESS,
    PIRATE,
    ADMIRAL
};

class Game;

class Specialist : public GameObject, public Possessable
{
private:
    SpecialistType type;

public:
    Specialist(){}
    Specialist(SpecialistType type) : type(type) {}
    void updatePointers(Game *game) override { Possessable::updatePointers(game); }

    SpecialistType getType() const { return type; }
};

#endif