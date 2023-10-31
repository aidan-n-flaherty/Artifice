#ifndef _BATTLE_EVENT_H_
#define _BATTLE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"
#include "../helpers/point.h"

class BattleEvent : public Event
{
private:
    std::unordered_map<std::string, std::list<std::pair<int, std::string>>> battleLog;
    std::unordered_map<std::string, std::pair<int, int>> phaseUnits;

    std::string currentPhase;

    PositionalObject* a;
    PositionalObject* b;

    int aID;
    int bID;

    int aOwnerID;
    int bOwnerID;

    int startingUnitsA = 0;
    int startingUnitsB = 0;

    int victorUnits = 0;

    Point location;

    int victorID = -1;

    bool friendly = false;

public:
    BattleEvent(){}
    BattleEvent(double timestamp, PositionalObject* a, PositionalObject* b) : Event(nullptr, timestamp), a(a), b(b), aID(a->getID()), bID(b->getID()), aOwnerID(a->getOwnerID()), bOwnerID(b->getOwnerID()), friendly(a->getOwnerID() == b->getOwnerID()) {}
    
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
        battleLog[currentPhase].push_back(std::pair<int, std::string>(userID, message));
    }

    void setPhaseUnits() {
        phaseUnits[currentPhase] = std::make_pair(a->getUnits(), b->getUnits());
    }

    void setLocation(const Point &location) { this->location = location; }
    const Point getLocation() const { return location; }

    void setVictor(int victorID) {
        this->victorID = victorID;
        setVictorUnits(victorID == aID ? a->getUnits() : b->getUnits());
    }
    int getVictor() const { return victorID; }

    int getStartingUnitsA() const { return startingUnitsA; }
    int getStartingUnitsB() const { return startingUnitsB; }

    void setVictorUnits(int units) { victorUnits = units; }
    int getVictorUnits() const { return victorUnits; }

    void setFriendly() { friendly = true; }
    bool isFriendly() const { return a->getOwnerID() == b->getOwnerID(); }

    void setPhase(const std::string &phase) {
        currentPhase = phase;
        battleLog[phase] = std::list<std::pair<int, std::string>>();
        setPhaseUnits();
    }

    static const std::list<std::string> getPhases() {
        return { "Specialist Phase", "Post-Specialist Phase", "Combat Resolution Phase", "Post-Combat Phase" };
    }

    const std::pair<int, int> getBattleUsers() const {
        return std::make_pair(aOwnerID, bOwnerID);
    }

    const std::unordered_map<int, int> getStartingUnits() const {
        return std::unordered_map<int, int> {
            { aOwnerID, startingUnitsA },
            { bOwnerID, startingUnitsB }
        };
    }

    const std::list<std::pair<int, std::string>>& getBattleLog(const std::string &phase) const {
        return battleLog.at(phase);
    }

    const std::unordered_map<int, int> getPhaseUnits(const std::string &phase) const {
        const std::pair<int, int>& p = phaseUnits.at(phase);
        return std::unordered_map<int, int> {
            { aOwnerID, p.first },
            { bOwnerID, p.second }
        };
    }
};

#endif