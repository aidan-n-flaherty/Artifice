#ifndef _ORDER_H_
#define _ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <set>
#include <string>

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

    double timestamp;

    int senderID;

public:
    Order() : ID(counter++), referenceID(-1) {};
    Order(double timestamp, int senderID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(-1) {}
    Order(double timestamp, int senderID, int referenceID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(referenceID) {}
    Order(int id, double timestamp, int senderID, int referenceID) : ID(id), timestamp(timestamp), senderID(senderID), referenceID(referenceID) {}

    virtual void adjustIDs(int createdID) {}

    void updateOrders(Game* game, const std::multiset<Order*, OrderOrder> &orders) const;

    double getTimestamp() const { return timestamp; }

    int getSenderID() const { return senderID; }

    int getID() const { return ID; }

    int getReferenceID() const { return referenceID; }

    /* IMPORTANT: All order validity preprocessing occurs in this function.
    ** It should be expected that no error checking occurs after the order is
    ** processed and converted to an event.
    */
    virtual Event* convert(Game* game) { return nullptr; }

    virtual std::string getType() { return ""; }

    bool operator<(const Order& other) const
    {
        double diff = other.getTimestamp() - getTimestamp();
        return diff == 0 ? getSenderID() < other.getSenderID() : diff > 0;
    }
};

#endif