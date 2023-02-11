#ifndef _SPECIALIST_H_
#define _SPECIALIST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include "../game_object.h"
#include "../possessable.h"

enum SpecialistType {
    QUEEN,
    PRINCESS,
    PIRATE,
};

class Specialist : public GameObject, public Possessable
{
private:
    SpecialistType type;

public:
    Specialist(){}
    Specialist(SpecialistType type) : type(type) {}

    SpecialistType getType() const { return type; }
};

#endif