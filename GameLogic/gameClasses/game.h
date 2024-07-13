#ifndef _GAME_H_
#define _GAME_H_

#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <ctime>
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <tuple>
#include "gameObjects/player.h"
#include "gameObjects/vessel.h"
#include "gameObjects/outpost.h"
#include "gameObjects/specialist.h"
#include "gameObjects/positional_object.h"
#include "game_settings.h"
#include "attack_notification.h"
#include "order.h"
#include "event.h"

class Game;

class BattleEvent;

class VesselOutpostEvent;

class WinConditionEvent;

class OutpostRangeEvent;

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
    void updateState(double timestamp);

    // client-side support for easy time machine usage
    void cacheState();

    bool ended = false;
    bool cacheEnabled = false;
    double startTime = 0.0;
    double stateTime = 0.0;
    double endTime = 0.0;
    double gameEndTime = 0.0;
    double nextEndState = 0.0;

    // client side variables to determine the most recent order sent by another player to use as a reference for IDs,
    // as well as the ID of the current player
    int referenceID = -1;
    int simulatorID = -1;

    int lastExecutedOrder = -1;
    int gameObjCounter = 0;
    int generatedObjCounter = 1000000000;

    std::unordered_map<int, Player*> players;
    std::unordered_map<int, Vessel*> vessels;
    std::unordered_map<int, Outpost*> outposts;
    std::unordered_map<int, Specialist*> specialists;

    std::multiset<Event*, EventOrder> events;
    std::multiset<Order*, OrderOrder> orders;
    std::vector<Event*> simulatedEvents;
    std::list<Order*> invalidOrders;
    std::list<Order*> simulatedOrders;

    std::multiset<std::shared_ptr<Game>, GameOrder> cache;

    std::set<AttackNotification*, NotificationOrder> notifications;

    std::unordered_map<int, double> ignoredVessels;

    std::unordered_set<int> removedVessels;

    GameSettings* settings = nullptr;

public:
    // Deterministically creates a pseudo-random map and initializes all player states.
    Game(){};
    Game(GameSettings settings, int simulatorID, double startTime, double endTime, std::map<int, std::tuple<std::string, int, int>>& playerInfo, int seed, bool cacheEnabled);
    Game(const Game& game);
    ~Game();

    // Uses the orders supplied and runs a full simulation of the game, returns player ids and rating changes
    std::list<std::pair<int, int>> run(bool pastEnd);

    std::list<std::pair<int, int>> run() { return run(true); };

    // returns a list of players sorted in order of ranking, paired with their corresponding rating change
    std::list<std::pair<int, int>> getScores();

    // sorts players by how close they are to victory
    std::vector<Player*> sortedPlayers(double timeDiff) const;

    bool hasEnded() const;
    bool simulationEnded() { return ended; }

    void endGame();

    Order* getOrder(const int id);
    Player* getPlayer(const int id) { return hasPlayer(id) ? players[id] : nullptr; }
    Vessel* getVessel(const int id) { return hasVessel(id) ? vessels[id] : nullptr; }
    Outpost* getOutpost(const int id) { return hasOutpost(id) ? outposts[id] : nullptr; }
    Specialist* getSpecialist(const int id) { return hasSpecialist(id) ? specialists[id] : nullptr; }
    PositionalObject* getPosObject(const int id) { if(hasOutpost(id)) return getOutpost(id); else return getVessel(id); }
    
    const std::unordered_map<int, Vessel*>& getVessels() const { return vessels; }
    const std::unordered_map<int, Outpost*>& getOutposts() const { return outposts; }
    const std::unordered_map<int, Player*>& getPlayers() const { return players; }
    const std::unordered_map<int, Specialist*>& getSpecialists() const { return specialists; }
    const std::multiset<Event*, EventOrder>& getEvents() const { return events; }
    const std::vector<Event*>& getSimulatedEvents() const { return simulatedEvents; }
    const std::multiset<Order*, OrderOrder>& getOrders() const { return orders; }
    const std::list<Order*>& getInvalid() { return invalidOrders; }
    const std::set<AttackNotification*, NotificationOrder>& getNotifications() { return notifications; };
    const std::unordered_map<int, double> getIgnoredVessels() { return ignoredVessels; }
    std::list<Outpost*> getTeamOutposts(int teamID) const;
    std::list<Vessel*> getTeamVessels(int teamID) const;
    
    bool hasPlayer(const int id) const { return players.find(id) != players.end(); }
    bool hasVessel(const int id) const { return vessels.find(id) != vessels.end(); }
    bool hasOutpost(const int id) const { return outposts.find(id) != outposts.end(); }
    bool hasSpecialist(const int id) const { return specialists.find(id) != specialists.end(); }
    bool hasPosObject(const int id) const { return hasOutpost(id) || hasVessel(id); }
    bool ignoreVessel(const int id, double timestamp) { return ignoredVessels.find(id) != ignoredVessels.end() && timestamp >= ignoredVessels[id]; }
    int simulatedEventCount() const { return simulatedEvents.size(); }
    int cacheSize() const { return cache.size(); }


    void addPlayer(Player* p);
    void addVessel(Vessel* v);
    void addOutpost(Outpost* o);
    void addSpecialist(Specialist* o);
    void addOrder(Order* o);
    void addEvent(Event* e);
    void addNotification(AttackNotification* n);

    std::shared_ptr<Game> removeOrder(int ID);
    std::shared_ptr<Game> adjustSend(int orderID, int units, const std::list<int>& specialistIDs);
    void addOrder(const std::string &type, int ID, int referenceID, bool canceled, double timestamp, int senderID, int argumentIDs[], int argCount);
    std::shared_ptr<Game> processOrder(const std::string &type, int ID, int referenceID, bool canceled, double timestamp, int senderID, int argumentIDs[], int argCount);
    
    bool removedVessel(int vesselID) { return removedVessels.find(vesselID) != removedVessels.end(); }
    void removeVessel(Vessel* v);
    void removeSpecialist(Specialist* s);

    void setEndTime(double t) { endTime = t; }
    
    double getStartTime() const { return startTime; }
    double getTime() const { return stateTime; }
    double getEndTime() const { return endTime; }
    double getGameEndTime() const { return gameEndTime; }

    int incrementGeneratedCounter() { return generatedObjCounter++; }

    int getGeneratedCounter() { return generatedObjCounter; }

    int incrementObjCounter() { return gameObjCounter++; }

    int getObjCounter() { return gameObjCounter; }

    GameSettings* getSettings() { return settings; };

    bool teamGame() const { return settings->number_of_teams > 1; }

    // The functions below should only be used by the client

    void setIgnoreVessel(int ID, double timestamp, bool ignore) { if(ignore) ignoredVessels[ID] = timestamp; else ignoredVessels.erase(ID); }
    std::shared_ptr<Game> setSimulateVessel(int ID, double timestamp, bool simulate);
    std::shared_ptr<Game> setSimulateOrder(int ID, bool simulate);
    std::list<int> ignoredOrders();
    bool canRetreat(Vessel* v, double timeDiff) const;
    bool withinRange(Player* p, PositionalObject* obj, double timeDiff) const;
    bool withinRange(Player* p, const Point& pos, double timeDiff) const;
    std::shared_ptr<Game> lastState(double timestamp);
    std::shared_ptr<Game> lastStateBefore(double timestamp);
    std::shared_ptr<Game> stateBefore(int orderID);
    double nextState(double timestamp);
    double getNextEndState() const { return nextEndState; }
    Event* nextAssociatedEvent(double timestamp, int id);
    const BattleEvent* nextBattle(int id, double timestamp);
    const WinConditionEvent* nextWinCondition(double timestamp);
    const VesselOutpostEvent* nextArrival(int id, double timestamp);
    const OutpostRangeEvent* nextFireEvent(double timestamp);
    std::list<BattleEvent*> nextBattles(int id);
    const BattleEvent* simulatedBattle(int eventID);

    int getReferenceID() { return referenceID; }
    int getSimulatorID() { return simulatorID; }

    int getLastExecutedOrder() { return lastExecutedOrder; }
};

#endif