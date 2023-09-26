#ifndef _BATTLE_EVENT_H_
#define _BATTLE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <tuple>
#include <algorithm>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"
#include "../helpers/point.h"

class BattleEvent : public Event
{
private:
    std::list<std::pair<int, std::string>> battleLog;

    PositionalObject* a;
    PositionalObject* b;

    int aID;
    int bID;

    int startingUnitsA;
    int startingUnitsB;

    Point location;

    int victorID = -1;

public:
    BattleEvent(){}
    BattleEvent(double timestamp, PositionalObject* a, PositionalObject* b) : Event(nullptr, timestamp), a(a), b(b), aID(a->getID()), bID(b->getID()) {}
    
    Event* copy() override { return new BattleEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);

        a = game->getPosObject(a->getID());
        b = game->getPosObject(b->getID());
    }

    bool referencesObject(int id) const override { return aID == id || bID == id; }

    void run(Game* game) override {
        startingUnitsA = a->getUnits();
        startingUnitsB = b->getUnits();
        
        setLocation(a->getPosition());
    }

    void specialistPhase(Game* game);
    void postSpecialistPhase(Game* game);
    void victorySpecialistPhase(Game* game);
    void defeatSpecialistPhase(Game* game);
    void postCombatSpecialistPhase(Game* game);

    void addMessage(int userID, const std::string &message) {
        battleLog.push_back(std::pair<int, std::string>(userID, message));
    }

    void setLocation(const Point &location) { this->location = location; }
    const Point getLocation() const { return location; }

    void setVictor(int victorID) { this->victorID = victorID; }
    int getVictor() const { return victorID; }

    int getStartingUnitsA() { return startingUnitsA; }
    int getStartingUnitsB() { return startingUnitsB; }
};

#endif