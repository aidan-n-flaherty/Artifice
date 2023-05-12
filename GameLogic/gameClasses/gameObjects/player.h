#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <set>
#include <list>
#include "../game_object.h"
#include "../event.h"

enum class SpecialistType;

class Game;

class Vessel;

class Outpost;

class Specialist;

class PositionalObject;

class Player : public GameObject
{
private:
    int userID;

    int rating;

    std::string name;

    bool defeated;
    time_t defeatedTime;

    std::list<std::shared_ptr<Specialist>> specialists;
    std::list<std::shared_ptr<Outpost>> outposts;
    std::list<std::shared_ptr<Vessel>> vessels;

    int resources;
    double fractionalProduction;

    int hires;
    double fractionalHires;

public:
    Player() {}
    Player(std::string name, int userID, int rating) : userID(userID), name(name), rating(rating), defeated(false), resources(0), fractionalProduction(0) {}
    
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
    time_t getDefeatedTime() const { return defeatedTime; }

    void setDefeated(Game* game);

    void projectedVictory(std::shared_ptr<Player> player, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);

    int specialistCount(SpecialistType t) const;
    bool controlsSpecialist(SpecialistType t) const;
    bool controlsSpecialists(std::list<int> specialists) const;

    std::list<std::shared_ptr<Specialist>> getSpecialists() const { return specialists; }
    void addSpecialist(std::shared_ptr<Specialist> specialist);
    void addSpecialists(std::list<std::shared_ptr<Specialist>> specialist);
    void removeSpecialist(std::shared_ptr<Specialist> specialist);

    std::list<std::shared_ptr<Outpost>> getOutposts() const { return outposts; }
    std::list<std::shared_ptr<Outpost>> sortedOutposts(PositionalObject* obj);
    void addOutpost(std::shared_ptr<Outpost> outpost);
    void removeOutpost(std::shared_ptr<Outpost> outpost);

    std::list<std::shared_ptr<Vessel>> getVessels() const { return vessels; }
    void addVessel(std::shared_ptr<Vessel> vessel);
    void removeVessel(std::shared_ptr<Vessel> vessel);

    void removeHire() { hires -= 1; }

    int getUserID() const { return userID; }
    const std::string& getName() const { return name; }

    int getHiresAt(double timeDiff) const;
    int getResourcesAt(double timeDiff) const;
    int getHiresAt(double& fractionalProduction, double timeDiff) const;
    int getResourcesAt(double& fractionalHires, double timeDiff) const;

    int getResources() const { return resources; }
    int getHires() const { return hires; }
    int getCapacity() const;
    int getUnits() const;

    void addResources(int amount) { resources += amount; }
    void removeResources(int amount) { resources -= amount; }
};

#endif