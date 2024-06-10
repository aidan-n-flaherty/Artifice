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

    const int ID = 0;

    // this variable is important to determine which game state this order was created off of
    const int referenceID = 0;

    double timestamp = 0.0;

    int senderID = -1;

    bool canceled = false;

    bool simulated = true;

    std::string description = "";

public:
    Order() : ID(counter++), referenceID(-1) {};
    Order(double timestamp, int senderID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(-1) {}
    Order(double timestamp, int senderID, int referenceID) : ID(counter++), timestamp(timestamp), senderID(senderID), referenceID(referenceID) {}
    Order(int id, double timestamp, int senderID, int referenceID, bool canceled) : ID(id), timestamp(timestamp), senderID(senderID), referenceID(referenceID), canceled(canceled) {}

    virtual void adjustIDs(int createdID, int amount) {}

    void updateOrders(Game* game, const std::multiset<Order*, OrderOrder> &orders) const;

    double getTimestamp() const { return timestamp; }

    int getSenderID() const { return senderID; }

    int getID() const { return ID; }

    int getReferenceID() const { return referenceID; }

    virtual int objIDDisplacement() { return 0; }

    void setCanceled(bool canceled) { this->canceled = canceled; }

    bool isCanceled() { return canceled || !simulated; }

    void setSimulated(bool simulated) { this->simulated = simulated; }

    bool isSimulated() { return simulated; }

    /* IMPORTANT: All order validity preprocessing occurs in this function.
    ** It should be expected that no error checking occurs after the order is
    ** processed and converted to an event.
    */
    virtual Event* converted(Game* game) { return nullptr; }

    virtual Event* convert(Game* game);

    virtual Order* copy() { return new Order(*this); }

    virtual std::string getType() const { return ""; }

    void setDescription(const std::string& str) { this->description = str; }

    std::string getDescription() const { return description != "" ? description : getType(); }

    bool operator<(const Order& other) const
    {
        double diff = other.getTimestamp() - getTimestamp();
        if(diff == 0) {
            int diff1 = other.getSenderID() - getSenderID();

            if(diff1 == 0) {
                return other.getID() > getID();
            } else return diff1 > 0;
        } else return diff > 0;
    }
};

#endif