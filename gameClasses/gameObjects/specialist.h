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
    ADMIRAL,
    NAVIGATOR,
    ASSASSIN,
    INFILTRATOR,
    LIEUTENANT,
    THIEF,
    DOUBLE_AGENT,
    INSPECTOR,
    MARTYR,
    REVERED_ELDER,
    SABOTEUR,
    SENTRY,
    SMUGGLER,
    DIPLOMAT,
    FOREMAN,
    HELMSMAN,
    HYPNOTIST,
    INTELLIGENCE_OFFICER,
    TINKERER,
    ENGINEER,
    GENERAL,
    KING,
    MINISTER_OF_ENERGY,
    SECURITY_CHIEF,
    TYCOON,
    WAR_HERO
};

class Game;

class Specialist : public GameObject, public Possessable
{
private:
    SpecialistType type;

    bool captured;

public:
    Specialist(){}
    Specialist(SpecialistType type) : type(type) {}
    
    void updatePointers(Game *game) override { Possessable::updatePointers(game); }

    SpecialistType getType() const { return type; }

    void setType(SpecialistType type) { this->type = type; }

    bool isCaptured() const { return captured; }

    void setCaptured(bool captured) { this->captured = captured; }
};

#endif