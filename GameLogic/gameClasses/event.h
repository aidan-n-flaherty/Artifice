#ifndef _EVENT_H_
#define _EVENT_H_

#include <cstdlib>
#include <cmath>
#include <time.h>
#include <memory>
#include "gameObjects/positional_object.h"
#include "order.h"

class Game;

/* This class manages how and when the game updates, similar to an uneven tick.
** If there isn't an event for it, then it doesn't happen.
*/
class Event
{
private:
    static int counter;

    const int ID = 0;
    
    Order* originatingOrder;

    double timestamp = 0.0;

public:
    Event() : ID(counter++) {}
    Event(Order* o, double timestamp) : originatingOrder(o), timestamp(timestamp), ID(counter++) {}

    virtual void updatePointers(Game *game) {}

    virtual void run(Game* game) {}

    virtual Event* copy() { return new Event(*this); }

    virtual bool referencesObject(int id) const { return false; }

    double getTimestamp() const { return timestamp; }

    bool operator<(const Event& other) const
    {
        return other.getTimestamp() - getTimestamp() > 0;
    }

    int getID() const { return ID; }

    bool operator()(Event* lhs, Event* rhs) const 
    {
        double diff = lhs->getTimestamp() - rhs->getTimestamp();
        return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
    }

    Order* getOriginatingOrder() { return originatingOrder; }
};

struct EventOrder
{
    bool operator()(const Event* lhs, const Event* rhs) const 
    {
        double diff = lhs->getTimestamp() - rhs->getTimestamp();
        return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
    }
};


#endif