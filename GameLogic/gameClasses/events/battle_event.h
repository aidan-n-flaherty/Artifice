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
    std::list<std::string> captures;

    std::string currentPhase;

    PositionalObject* a;
    PositionalObject* b;

    int aID;
    int bID;

    int aOwnerID;
    int bOwnerID;

    int startingUnitsA = 0;
    int startingUnitsB = 0;

    int preVictoryUnitsA = 0;
    int preVictoryUnitsB = 0;

    int shieldsA = 0;
    int shieldsB = 0;

    int victorUnits = 0;

    Point location;

    int victorID = -1;

    bool friendly = false;

public:
    BattleEvent(){}
    BattleEvent(double timestamp, PositionalObject* a, PositionalObject* b) : Event(nullptr, timestamp),
        a(a), b(b), aID(a->getID()), bID(b->getID()), aOwnerID(a->getOwnerID()), bOwnerID(b->getOwnerID()),
        friendly(a->getOwnerID() == b->getOwnerID()) {}
    
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

        std::list<Specialist*> captured;
        if(victorID == aOwnerID) {
            setVictorUnits(a->getUnits());
            captured = b->getSpecialists();
        } else if(victorID == bOwnerID) {
            setVictorUnits(b->getUnits());
            captured = a->getSpecialists();
        }

        for(Specialist* s : captured) {
            std::string str = s->typeAsString();
            std::replace(str.begin(), str.end(), '_', ' ');
            captures.push_back(str);
        }
    }
    int getVictor() const { return victorID; }

    void setVictorUnits(int units) { victorUnits = units; }
    int getVictorUnits() const { return victorUnits; }

    std::list<std::string> getCaptures() const { return captures; }

    void setFriendly() { friendly = true; }
    bool isFriendly() const { return friendly; }

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

    void setPreVictoryUnits(int ownerID, int amount) {
        if(ownerID == aOwnerID) preVictoryUnitsA = amount;
        else if(ownerID == bOwnerID) preVictoryUnitsB = amount;
    }

    const std::unordered_map<int, int> getPreVictoryUnits() const {
        return std::unordered_map<int, int> {
            { aOwnerID, preVictoryUnitsA },
            { bOwnerID, preVictoryUnitsB }
        };
    }

    void setShields(int ownerID, int amount) {
        if(ownerID == aOwnerID) shieldsA = amount;
        else if(ownerID == bOwnerID) shieldsB = amount;
    }

    const std::unordered_map<int, int> getShields() const {
        return std::unordered_map<int, int> {
            { aOwnerID, shieldsA },
            { bOwnerID, shieldsB }
        };
    }

    const std::list<std::pair<int, std::string>> getBattleLog(const std::string &phase) const {
        return battleLog.find(phase) != battleLog.end() ? battleLog.at(phase) : std::list<std::pair<int, std::string>>();
    }

    const std::unordered_map<int, int> getPhaseUnits(const std::string &phase) const {
        const std::pair<int, int>& p = phaseUnits.find(phase) != phaseUnits.end() ? phaseUnits.at(phase) : std::make_pair(0, 0);
        return std::unordered_map<int, int> {
            { aOwnerID, p.first },
            { bOwnerID, p.second }
        };
    }
};

#endif