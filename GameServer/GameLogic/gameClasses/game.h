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
class Game : public std::enable_shared_from_this<Game>
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
    int referenceID = -1;
    int simulatorID = -1;

    std::unordered_map<int, Player*> players;
    std::unordered_map<int, Vessel*> vessels;
    std::unordered_map<int, Outpost*> outposts;
    std::unordered_map<int, Specialist*> specialists;

    std::multiset<Event*, EventOrder> events;
    std::multiset<Order*, OrderOrder> orders;
    std::list<Event*> simulatedEvents;
    std::list<Order*> invalidOrders;

    std::multiset<std::shared_ptr<Game>, GameOrder> cache;

public:
    // Deterministically creates a pseudo-random map and initializes all player states.
    Game(){};
    Game(int simulatorID, time_t startTime, time_t endTime, std::map<int, std::string>& players, int seed, bool cacheEnabled);
    Game(const Game& game);
    ~Game();

    // Uses the orders supplied and runs a full simulation of the game, returns player ids and rating changes
    std::list<std::pair<int, int>> run();

    // sorts players by how close they are to victory
    std::vector<Player*> sortedPlayers() const;

    bool hasEnded() const;

    void endGame();

    Player* getPlayer(const int id) { return players[id]; }
    Vessel* getVessel(const int id) { return vessels[id]; }
    Outpost* getOutpost(const int id) { return outposts[id]; }
    Specialist* getSpecialist(const int id) { return specialists[id]; }
    PositionalObject* getPosObject(const int id) { if(hasOutpost(id)) return getOutpost(id); else return getVessel(id); }
    
    const std::unordered_map<int, Vessel*>& getVessels() const { return vessels; }
    const std::unordered_map<int, Outpost*>& getOutposts() const { return outposts; }
    const std::unordered_map<int, Player*>& getPlayers() const { return players; }
    const std::unordered_map<int, Specialist*>& getSpecialists() const { return specialists; }
    const std::multiset<Event*, EventOrder>& getEvents() const { return events; }
    const std::multiset<Order*, OrderOrder>& getOrders() const { return orders; }
    const std::list<Order*>& getInvalid() { return invalidOrders; }
    
    bool hasPlayer(const int id) const { return players.count(id) != 0; }
    bool hasVessel(const int id) const { return vessels.count(id) != 0; }
    bool hasOutpost(const int id) const { return outposts.count(id) != 0; }
    bool controlsSpecialist(const int id) const { return specialists.count(id) != 0; }
    bool hasPosObject(const int id) const { return hasOutpost(id) || hasVessel(id); }
    bool controlsSpecialist(SpecialistType t, std::list<int> specialists) const; 
    int simulatedEventCount() const { return simulatedEvents.size(); }

    void addPlayer(Player* p);
    void addVessel(Vessel* v);
    void addOutpost(Outpost* o);
    void addSpecialist(Specialist* o);
    void addOrder(Order* o);
    void addEvent(Event* e);

    std::shared_ptr<Game> processOrder(const std::string &type, int ID, int referenceID, long timestamp, int senderID, int argumentIDs[], int argCount);
    
    void removeVessel(Vessel* v);
    void removeSpecialist(Specialist* s);
    
    time_t getTime() const { return stateTime; }
    time_t getEndTime() const { return endTime; }

    // These functions are strictly for client side rendering
    std::shared_ptr<Game> lastState(time_t timestamp);
    time_t nextState(time_t timestamp);
    Event* nextAssociatedEvent(time_t timestamp, int id);

    int getReferenceID() { return referenceID; };
    int getSimulatorID() { return simulatorID; };
};

#endif