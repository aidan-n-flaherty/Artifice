#ifndef _ORDER_H_
#define _ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>

enum OrderType {
    SEND, // params: [originID, targetID, numUnits, list of specialist IDs]
    HIRE, // params: [specialistID]
    PROMOTE // params: [specialistID]
};

class Order
{
private:
    time_t timestamp;

    int senderID;

    OrderType order;

    std::list<int> params;

public:
    Order(){};
    Order(time_t timestamp, int senderID, OrderType order, std::list<int> params) : timestamp(timestamp), senderID(senderID), order(order), params(params) {}

    time_t getTimestamp() const { return timestamp; }

    std::list<int>& getParams() { return params; }

    int getSenderID(){ return senderID; }

    OrderType getType() { return order; }

    bool operator<(const Order& other) const
    {
        return difftime(other.getTimestamp(), getTimestamp()) > 0;
    }
};

#endif