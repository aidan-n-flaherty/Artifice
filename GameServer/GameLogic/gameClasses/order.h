#ifndef _ORDER_H_
#define _ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <set>

enum class OrderType {
    SEND, // params: [originID, targetID, numUnits, list of specialist IDs]
    REROUTE, // params: [vesselID, targetID]
    HIRE, // params: [specialistID]
    PROMOTE // params: [specialistID]
};

class Order;

struct OrderOrder {
    bool operator()(Order* lhs, Order* rhs) const;
};

class Event;

class Game;

class Order
{
private:
    static int counter;

    const int ID;

    // this variable is important to determine which game state this order was created off of
    const int referenceID;

    time_t timestamp;

    int senderID;

public:
    Order() : ID(counter++), referenceID(-1) {};
    Order(time_t timestamp, int senderID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(-1) {}
    Order(time_t timestamp, int senderID, int referenceID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(referenceID) {}
    Order(int id, time_t timestamp, int senderID, int referenceID) : ID(id), timestamp(timestamp), senderID(senderID), referenceID(referenceID) {}

    virtual void adjustIDs(int createdID) {}

    void updateOrders(const std::multiset<Order*, OrderOrder> &orders) const;

    time_t getTimestamp() const { return timestamp; }

    int getSenderID() const { return senderID; }

    int getID() const { return ID; }

    int getReferenceID() const { return referenceID; }

    /* IMPORTANT: All order validity preprocessing occurs in this function.
    ** It should be expected that no error checking occurs after the order is
    ** processed and converted to an event.
    */
    virtual Event* convert(Game* game) { return nullptr; }

    bool operator<(const Order& other) const
    {
        double diff = difftime(other.getTimestamp(), getTimestamp());
        return diff == 0 ? getSenderID() < other.getSenderID() : diff > 0;
    }
};

#endif