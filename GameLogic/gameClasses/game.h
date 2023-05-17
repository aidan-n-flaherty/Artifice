#ifndef _GAME_H_
#define _GAME_H_

#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <list>
#include <ctime>
#include <set>
#include <map>
#include "gameObjects/player.h"
#include "gameObjects/vessel.h"
#include "gameObjects/outpost.h"
#include "gameObjects/specialist.h"
#include "order.h"
#include "event.h"
#include "gameObjects/positional_object.h"

class Game;

struct GameOrder {
    bool operator()(const std::shared_ptr<Game> &lhs, const std::shared_ptr<Game> &rhs) const;
};

/* Contains all the data necessary to run a complete simulation of a game.
** Note that on the server-side, this class should be run once and then immediately discarded.
*/
class Game
{
private:
    // Given a game element flagged for modification, discards all events it's associated with.
    void removeRelevant(int id);

    // Overwrites outdated events for modified game elements.
    void updateEvents();

    // Moves the game state a specific number of seconds into the future.
    void updateState(time_t timestamp);

    // client-side support for easy time machine usage
    void cacheState();

    // returns a list of players sorted in order of ranking, paired with their corresponding rating change
    std::list<std::pair<int, int>> getScores();

    bool cacheEnabled;
    time_t stateTime;
    time_t endTime;

    // client side variables to determine the most recent order sent by another player to use as a reference for IDs,
    // as well as the ID of the current player
    int referenceID;
    int simulatorID = -1;

    std::unordered_map<int, std::shared_ptr<Player>> players;
    std::unordered_map<int, std::shared_ptr<Vessel>> vessels;
    std::unordered_map<int, std::shared_ptr<Outpost>> outposts;
    std::unordered_map<int, std::shared_ptr<Specialist>> specialists;

    std::multiset<std::shared_ptr<Event>, EventOrder> events;
    std::multiset<std::shared_ptr<Order>, OrderOrder> orders;
    std::list<std::shared_ptr<Event>> simulatedEvents;
    std::list<std::shared_ptr<Order>> invalidOrders;

    std::set<std::shared_ptr<Game>, GameOrder> cache;

public:
    // Deterministically creates a pseudo-random map and initializes all player states.
    Game(){};
    Game(int simulatorID, time_t startTime, time_t endTime, std::map<int, std::string>& players, int seed, bool cacheEnabled);
    Game(const Game& game);

    // Uses the orders supplied and runs a full simulation of the game, returns player ids and rating changes
    std::list<std::pair<int, int>> run();

    // sorts players by how close they are to victory
    std::vector<std::shared_ptr<Player>> sortedPlayers() const;

    bool hasEnded() const;

    void endGame();

    std::shared_ptr<Player> getPlayer(const int id) { return players[id]; }
    std::shared_ptr<Vessel> getVessel(const int id) { return vessels[id]; }
    std::shared_ptr<Outpost> getOutpost(const int id) { return outposts[id]; }
    std::shared_ptr<Specialist> getSpecialist(const int id) { return specialists[id]; }
    std::shared_ptr<PositionalObject> getPosObject(const int id) { if(hasOutpost(id)) return getOutpost(id); else return getVessel(id); }
    
    const std::unordered_map<int, std::shared_ptr<Vessel>>& getVessels() const { return vessels; }
    const std::unordered_map<int, std::shared_ptr<Outpost>>& getOutposts() const { return outposts; }
    const std::unordered_map<int, std::shared_ptr<Player>>& getPlayers() const { return players; }
    const std::unordered_map<int, std::shared_ptr<Specialist>>& getSpecialists() const { return specialists; }
    const std::multiset<std::shared_ptr<Event>, EventOrder>& getEvents() const { return events; }
    const std::multiset<std::shared_ptr<Order>, OrderOrder>& getOrders() const { return orders; }
    const std::list<std::shared_ptr<Order>>& getInvalid() { return invalidOrders; }
    
    bool hasPlayer(const int id) const { return players.count(id) != 0; }
    bool hasVessel(const int id) const { return vessels.count(id) != 0; }
    bool hasOutpost(const int id) const { return outposts.count(id) != 0; }
    bool controlsSpecialist(const int id) const { return specialists.count(id) != 0; }
    bool hasPosObject(const int id) const { return hasOutpost(id) || hasVessel(id); }
    bool controlsSpecialist(SpecialistType t, std::list<int> specialists) const; 

    void addPlayer(Player* p);
    void addVessel(Vessel* v);
    void addOutpost(Outpost* o);
    void addSpecialist(Specialist* o);
    void addOrder(Order* o);
    void addEvent(Event* e);

    std::shared_ptr<Game> processOrder(const std::string &type, int ID, int referenceID, long timestamp, int senderID, int argumentIDs[], int argCount);
    
    void removeVessel(std::shared_ptr<Vessel> v);
    void removeSpecialist(std::shared_ptr<Specialist> s);
    
    time_t getTime() const { return stateTime; }
    time_t getEndTime() const { return endTime; }

    // These functions are strictly for client side rendering
    std::shared_ptr<Game> lastState(time_t timestamp) const;
    time_t nextState(time_t timestamp);
    const std::shared_ptr<Event> nextAssociatedEvent(time_t timestamp, int id);

    int getReferenceID() { return referenceID; };
    int getSimulatorID() { return simulatorID; };
};

#endif