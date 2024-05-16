#ifndef _SPECIALIST_H_
#define _SPECIALIST_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <string>
#include <algorithm>
#include "../game_object.h"
#include "../possessable.h"

enum SpecialistType : unsigned int {
    NONE,
    QUEEN,
    PRINCESS,
    PIRATE,
    ADMIRAL,
    NAVIGATOR,
    ASSASSIN,
    INFILTRATOR,
    LIEUTENANT,
    THIEF,
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

class GameSettings;

class PositionalObject;

class Player;

class Specialist : public GameObject, public Possessable
{
private:
    SpecialistType type;

    PositionalObject* container = nullptr;

    int charge = 0;

public:
    Specialist(unsigned int ID, GameSettings* settings, SpecialistType type) : GameObject(ID, settings), type(type) {}
    
    void updatePointers(Game *game) override;

    SpecialistType getType() const { return type; }

    PositionalObject* getContainer() { return container; }

    void setType(SpecialistType type) { this->type = type; }

    virtual void setOwner(Player* player) override;

    void setContainer(PositionalObject* o) { container = o; }

    int getCharge() { return charge; }

    void consumeCharge() { charge--; }

    void recharge() {}

    static std::list<SpecialistType> allHires() {
        return {
            PRINCESS,
            PIRATE,
            ADMIRAL,
            NAVIGATOR,
            ASSASSIN,
            INFILTRATOR,
            LIEUTENANT,
            THIEF,
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
    }

    static std::list<SpecialistType> baseHires() {
        return {
            PRINCESS,
            NAVIGATOR,
            ASSASSIN,
            INFILTRATOR,
            LIEUTENANT,
            THIEF,
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
            TINKERER
        };
    }

    std::list<SpecialistType> promotionOptions() {
        return Specialist::promotionOptions(type);
    }

    static std::list<SpecialistType> promotionOptions(SpecialistType t) {
        switch(t){
            case NAVIGATOR: return { ADMIRAL };
            case FOREMAN: return { ENGINEER };
            case LIEUTENANT: return { GENERAL };
            case HYPNOTIST: return { KING };
            case TINKERER: return { MINISTER_OF_ENERGY };
            case INSPECTOR: return { SECURITY_CHIEF };
            case SMUGGLER: return { TYCOON };
            case SENTRY: return { WAR_HERO };
            case THIEF: return { PIRATE };
            default: return {};
        }
    }

    static int hireAmount(SpecialistType t) {
        switch(t){
            case ASSASSIN: case SABOTEUR: return 2;
            default: return 1;
        }
    }

    std::string typeAsString(){
        return Specialist::typeAsString(type);
    }

    static std::string typeAsString(SpecialistType t) {
        const std::string list[]{
            "NONE",
            "Queen",
            "Princess",
            "Pirate",
            "Admiral",
            "Navigator",
            "Assassin",
            "Infiltrator",
            "Lieutenant",
            "Thief",
            "Inspector",
            "Martyr",
            "Revered_Elder",
            "Saboteur",
            "Sentry",
            "Smuggler",
            "Diplomat",
            "Foreman",
            "Helmsman",
            "Hypnotist",
            "Intelligence_Officer",
            "Tinkerer",
            "Engineer",
            "General",
            "King",
            "Minister_Of_Energy",
            "Security_Chief",
            "Tycoon",
            "War_Hero"
        };

        return list[t];
    }
};

#endif