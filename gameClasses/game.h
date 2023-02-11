#ifndef _GAME_H_
#define _GAME_H_

#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <list>
#include <ctime>
#include <set>
#include "player.h"
#include "gameObjects/vessel.h"
#include "gameObjects/outpost.h"
#include "gameObjects/specialist.h"
#include "order.h"
#include "event.h"
#include "positional_object.h"

/* Contains all the data necessary to run a complete simulation of a game.
** Note that this class should be run once and then immediately discarded.
*/
class Game
{
private:
    // Convert an order to an event. Can only be run after all past events have been simulated.
    Event* processOrder(Order &order);

    // Given a game element flagged for modification, discards all events it's associated with.
    void removeRelevant(int id);

    // Overwrites outdated events for modified game elements.
    void updateEvents();

    // Moves the game state a specific number of seconds into the future.
    void updateState(time_t timestamp);

    int idCounter;
    time_t stateTime;

    std::unordered_map<int, Player> players;
    std::unordered_map<int, Vessel> vessels;
    std::unordered_map<int, Outpost> outposts;
    std::unordered_map<int, Specialist> specialists;

    std::multiset<Event*> events;

    int width;
    int height;

public:
    // Deterministically creates a pseudo-random map and initializes all player states.
    Game(){};
    Game(time_t startTime, std::list<Player> &players, int seed);

    // Assuming that init has already been run, takes all orders issued and simulates the game.
    std::list<int> run(std::multiset<Order> &orders, std::list<Order> &invalid);

    Player& getPlayer(const int id) { return players[id]; }
    Vessel& getVessel(const int id) { return vessels[id]; }
    Outpost& getOutpost(const int id) { return outposts[id]; }
    Specialist& getSpecialist(const int id) { return specialists[id]; }
    PositionalObject &getPosObject(const int id) { if(hasOutpost(id)) return getOutpost(id); else return getVessel(id); }

    bool hasPlayer(const int id) const { return players.count(id) != 0; }
    bool hasVessel(const int id) const { return vessels.count(id) != 0; }
    bool hasOutpost(const int id) const { return outposts.count(id) != 0; }
    bool hasPosObject(const int id) const { return hasOutpost(id) || hasVessel(id); }
    bool hasSpecialist(SpecialistType t, std::list<int> specialists) const; 

    void addPlayer(Player &p);
    void addVessel(Vessel &v);
    void addOutpost(Outpost &o);
    void addOrder(Order &o);
    
    void removeVessel(Vessel &v);
    void removePlayer(Player &p);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    time_t getTime() const { return stateTime; }
};

#endif