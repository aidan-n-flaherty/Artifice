#ifndef _SPECIALIST_H_
#define _SPECIALIST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <string>
#include "../game_object.h"
#include "../possessable.h"

enum SpecialistType {
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

class PositionalObject;

class Player;

class Specialist : public GameObject, public Possessable
{
private:
    SpecialistType type;

    PositionalObject* container = nullptr;

public:
    Specialist(){}
    Specialist(SpecialistType type) : type(type) {}
    
    void updatePointers(Game *game) override;

    SpecialistType getType() const { return type; }

    PositionalObject* getContainer() { return container; }

    void setType(SpecialistType type) { this->type = type; }

    virtual void setOwner(Player* player) override;

    void setContainer(PositionalObject* o) { container = o; }

    std::string typeAsString() {
        return (const std::string[]) {
            "Queen",
            "Princess",
            "Pirate",
            "Admiral",
            "Navigator",
            "Assassin",
            "Infiltrator",
            "Lieutenant",
            "Thief",
            "DoubleAgent",
            "Inspector",
            "Martyr",
            "ReveredElder",
            "Saboteur",
            "Sentry",
            "Smuggler",
            "Diplomat",
            "Foreman",
            "Helmsman",
            "Hypnotist",
            "IntelligenceOfficer",
            "Tinkerer",
            "Engineer",
            "General",
            "King",
            "MinisterOfEnergy",
            "SecurityChief",
            "Tycoon",
            "WarHero"
        }[type];
    }
};

#endif