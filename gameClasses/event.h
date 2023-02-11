#ifndef _EVENT_H_
#define _EVENT_H_

#include <cstdlib>
#include <cmath>
#include <time.h>

class Game;

/* This class manages how and when the game updates, similar to an uneven tick.
** If there isn't an event for it, then it doesn't happen.
*/
class Event
{
private:
    time_t timestamp;

public:
    Event(){}
    Event(time_t timestamp) : timestamp(timestamp) {}

    virtual void run(Game* game) const {}

    virtual bool referencesObject(int id) const { return false; }

    virtual bool valid() { return true; }

    time_t getTimestamp() const { return timestamp; }

    bool operator<(const Event& other) const
    {
        return difftime(other.getTimestamp(), getTimestamp()) > 0;
    }

    bool operator()(const Event* lhs, const Event* rhs) const  {
        return difftime(lhs->getTimestamp(), rhs->getTimestamp()) < 0;
    }
};

#endif