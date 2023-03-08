#ifndef _EVENT_H_
#define _EVENT_H_

#include <cstdlib>
#include <cmath>
#include <time.h>
#include <memory>
#include "gameObjects/positional_object.h"

class Game;

/* This class manages how and when the game updates, similar to an uneven tick.
** If there isn't an event for it, then it doesn't happen.
*/
class Event
{
private:
    static int counter;

    const int ID;
    
    time_t timestamp;

public:
    Event() : ID(counter++) {}
    Event(time_t timestamp) : timestamp(timestamp), ID(counter++) {}
    Event(const std::shared_ptr<Event> other, Game* game);

    virtual void updatePointers(Game *game) {}

    virtual void run(Game* game) const {}

    virtual bool referencesObject(int id) const { return false; }

    time_t getTimestamp() const { return timestamp; }

    bool operator<(const Event& other) const
    {
        return difftime(other.getTimestamp(), getTimestamp()) > 0;
    }

    int getID() const { return ID; }

    bool operator()(const std::shared_ptr<Event> &lhs, const std::shared_ptr<Event> &rhs) const 
    {
        double diff = difftime(lhs->getTimestamp(), rhs->getTimestamp());
        return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
    }
};

struct EventOrder
{
    bool operator()(const std::shared_ptr<Event> &lhs, const std::shared_ptr<Event> &rhs) const 
    {
        double diff = difftime(lhs->getTimestamp(), rhs->getTimestamp());
        return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
    }
};


#endif