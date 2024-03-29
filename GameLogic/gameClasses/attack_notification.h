#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#include <cstdlib>
#include <cmath>
#include <time.h>
#include <memory>
#include <list>

class AttackNotification
{
private:
    unsigned int listenerID;

    int vesselID;

    double timestamp = 0.0;

public:
    AttackNotification() {}
    AttackNotification(double timestamp, int vesselID, unsigned int listenerID) : vesselID(vesselID), listenerID(listenerID), timestamp(timestamp) {}

    double getTimestamp() const { return timestamp; }

    unsigned int getListenerID() const { return listenerID; }

    int getVesselID() { return vesselID; }
};

struct NotificationOrder
{
    bool operator()(const AttackNotification* lhs, const AttackNotification* rhs) const 
    {
        double diff = lhs->getTimestamp() - rhs->getTimestamp();
        return diff == 0 ? lhs->getListenerID() < rhs->getListenerID() : diff < 0;
    }
};


#endif