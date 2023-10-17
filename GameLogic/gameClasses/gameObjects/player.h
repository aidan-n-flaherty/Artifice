#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <set>
#include <list>
#include <algorithm>
#include "../game_object.h"
#include "../event.h"
#include "../game_settings.h"

enum SpecialistType : unsigned int;

enum OutpostType : unsigned int;

class Game;

class Vessel;

class Outpost;

class Specialist;

class PositionalObject;

class Player : public GameObject
{
private:
    int userID = 0;

    int rating = 0;

    std::string name;

    bool defeated = false;
    double defeatedTime = 0.0;

    std::list<Specialist*> specialists;
    std::list<Outpost*> outposts;
    std::list<Vessel*> vessels;

    int resources = 0;
    double fractionalProduction = 0.0;

    int hires = 10; // should be 1 in the future
    double fractionalHires = 0.0;

    int minesDrilled = 0;

public:
    Player(unsigned int ID, GameSettings* settings, std::string name, int userID, int rating) : GameObject(ID, settings), userID(userID), name(name), rating(rating), defeated(false) {}
    
    void updatePointers(Game* game);

    void update(double timeDiff);

    int getRating() const { return rating; }

    double globalSpeed() const;
    int globalMaxShield() const;
    int globalProductionAmount() const;
    double globalProductionSpeed() const;
    double globalSonar() const;
    double resourceProductionSpeed() const;

    bool hasLost() const { return defeated; }
    double getDefeatedTime() const { return defeatedTime; }

    void setDefeated(Game* game);

    void projectedVictory(Player* player, double timestamp, std::multiset<Event*, EventOrder> &events);

    int specialistCount(SpecialistType t) const;
    bool controlsSpecialist(SpecialistType t) const;
    bool controlsSpecialists(std::list<int> specialists) const;

    PositionalObject* getSpawnLocation();

    std::list<Specialist*> getSpecialists() const { return specialists; }
    void addSpecialist(Specialist* specialist);
    void addSpecialists(std::list<Specialist*> specialist);
    void removeSpecialist(Specialist* specialist);

    std::list<Outpost*> getOutposts() const { return outposts; }
    std::list<Outpost*> sortedOutposts(const PositionalObject* obj);
    void addOutpost(Outpost* outpost);
    void removeOutpost(Outpost* outpost);

    std::list<Vessel*> getVessels() const { return vessels; }
    void addVessel(Vessel* vessel);
    void removeVessel(Vessel* vessel);

    int outpostsOfType(OutpostType t) const;

    void removeHire() { hires -= 1; }
    void drillMine() { minesDrilled++; }
    int getMineCost() { return getSettings()->costPerMine * (minesDrilled + 1); }

    int getUserID() const { return userID; }
    const std::string& getName() const { return name; }

    int getUnitsAt(double timeDiff) const;
    int getHiresAt(double timeDiff) const;
    int getResourcesAt(double timeDiff) const;
    int getHiresAt(double& fractionalProduction, double timeDiff) const;
    int getResourcesAt(double& fractionalHires, double timeDiff) const;

    int getResources() const { return resources; }
    int getHires() const { return hires; }
    int getCapacity() const;
    int getUnits() const;
    int getMinesDrilled() const { return minesDrilled; }

    void addResources(int amount) { resources += amount; }
    void removeResources(int amount) { resources -= amount; }
};

#endif