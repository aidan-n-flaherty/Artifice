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

public:
    Player() {}
    Player(std::string name, int userID, int rating) : userID(userID), name(name), rating(rating), defeated(false), resources(0), fractionalProduction(0) {}
    
    void updatePointers(Game* game);

    void update(double timeDiff);

    int getRating() const { return rating; }

    double globalSpeed();
    int globalMaxShield();
    int globalProductionAmount();
    double globalProductionSpeed();
    double globalSonar();
    double resourceProductionSpeed();

    bool hasLost() const { return defeated; }
    time_t getDefeatedTime() const { return defeatedTime; }

    void setDefeated(Game* game);

    void projectedVictory(std::shared_ptr<Player> player, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events);

    int specialistCount(SpecialistType t) const;
    bool controlsSpecialist(SpecialistType t) const;
    bool controlsSpecialists(std::list<int> specialists);

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

    int getUserID() const { return userID; }
    const std::string& getName() const { return name; }

    int getResources() const { return resources; }
    int getCapacity() const;
    int getUnits() const;

    void addResources(int amount) { resources += amount; }
    void removeResources(int amount) { resources -= amount; }
};

#endif