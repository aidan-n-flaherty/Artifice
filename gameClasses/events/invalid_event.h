#ifndef _INVALID_EVENT_H_
#define _INVALID_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../game.h"

class InvalidEvent : public Event
{
public:
    InvalidEvent(){};
    InvalidEvent(time_t timestamp) : Event(timestamp) {}

    bool valid() override { return false; }
};

#endif