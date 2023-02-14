#ifndef _GAME_H_
#define _GAME_H_

#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <list>
#include <ctime>
#include <set>
#include <map>
#include "player.h"
#include "gameObjects/vessel.h"
#include "gameObjects/outpost.h"
#include "gameObjects/specialist.h"
#include "order.h"
#include "event.h"
#include "positional_object.h"

class Game;

struct GameOrder {
    bool operator()(const std::shared_ptr<Game> &lhs, const std::shared_ptr<Game> &rhs) const;
};

/* Contains all the data necessary to run a complete simulation of a game.
** Note that this class should be run once and then immediately discarded.
*/
class Game
{
private:
    // Convert an order to an event. Can only be run after all past events have been simulated.
    std::shared_ptr<Event> processOrder(Order &order);

    // Given a game element flagged for modification, discards all events it's associated with.
    void removeRelevant(int id);

    // Overwrites outdated events for modified game elements.
    void updateEvents();

    // Moves the game state a specific number of seconds into the future.
    void updateState(time_t timestamp);

    // client-side support for easy time machine usage
    void cacheState();

    bool cacheEnabled;
    int idCounter;
    time_t stateTime;
    int width;
    int height;

    std::unordered_map<int, std::shared_ptr<Player>> players;
    std::unordered_map<int, std::shared_ptr<Vessel>> vessels;
    std::unordered_map<int, std::shared_ptr<Outpost>> outposts;
    std::unordered_map<int, std::shared_ptr<Specialist>> specialists;

    std::multiset<std::shared_ptr<Event>, EventOrder> events;
    std::multiset<Order> orders;
    std::list<Order> invalidOrders;

    std::set<std::shared_ptr<Game>, GameOrder> cache;

public:
    // Deterministically creates a pseudo-random map and initializes all player states.
    Game(){};
    Game(time_t startTime, std::map<int, std::string>& players, int seed, bool cacheEnabled);
    Game(const Game& game);

    // Assuming that init has already been run, takes all orders issued and simulates the game.
    std::list<int> run();

    std::list<Order>& getInvalid() { return invalidOrders; }
    std::shared_ptr<Player> getPlayer(const int id) { return players[id]; }
    std::shared_ptr<Vessel> getVessel(const int id) { return vessels[id]; }
    std::shared_ptr<Outpost> getOutpost(const int id) { return outposts[id]; }
    std::shared_ptr<Specialist> getSpecialist(const int id) { return specialists[id]; }
    std::shared_ptr<PositionalObject> getPosObject(const int id) { if(hasOutpost(id)) return getOutpost(id); else return getVessel(id); }
    const std::unordered_map<int, std::shared_ptr<Vessel>>& getVessels() { return vessels; }

    bool hasPlayer(const int id) const { return players.count(id) != 0; }
    bool hasVessel(const int id) const { return vessels.count(id) != 0; }
    bool hasOutpost(const int id) const { return outposts.count(id) != 0; }
    bool hasPosObject(const int id) const { return hasOutpost(id) || hasVessel(id); }
    bool hasSpecialist(SpecialistType t, std::list<int> specialists) const; 

    void addPlayer(Player* p);
    void addVessel(Vessel* v);
    void addOutpost(Outpost* o);
    void addOrder(Order o);
    
    void removeVessel(std::shared_ptr<Vessel> v);
    void removePlayer(std::shared_ptr<Player> p);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Point getDimensions() { return Point(width, height); }
    time_t getTime() const { return stateTime; }

    std::shared_ptr<Game> lastState(time_t timestamp);
};

#endif