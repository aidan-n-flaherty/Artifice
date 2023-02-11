#include "vessel.h"
#include "../events/intervessel_event.h"
#include "../events/vessel_outpost_event.h"
#include <set>
#include <iostream>
#include <cmath>

/* Calculates the point that this vessel is currently targeting, taking the target's
** movement into account.
** Uses law of sines to derive answer.
*/
const Point Vessel::getTargetPos(const Point &dimensions) const {
    // return the target position if the target does not move
    const Point& targetPos = getPosition().closest(dimensions, target->getPosition());
    if(getSpeed() == 0 || target->getSpeed() == 0) return targetPos;

    const Point& targetTarget = targetPos.closest(dimensions, target->getTargetPos(dimensions));

    /* Construct a triangle with sides A: x, B: yx, and C: z, where y is the ratio between
    ** sides B and A, and z is the current distance between the vessels. We know the angle 
    ** between A and C.
    ** x is the unknown, and determines the point of intersection.
    */
    double ratio = getSpeed()/target->getSpeed();

    Point vec1(targetTarget.getX() - targetPos.getX(), targetTarget.getY() - targetPos.getY());
    double mag1 = sqrt(vec1.getX() * vec1.getX() + vec1.getY() * vec1.getY());

    Point vec2(getPosition().getX() - targetPos.getX(), getPosition().getY() - targetPos.getY());
    double mag2 = sqrt(vec2.getX() * vec2.getX() + vec2.getY() * vec2.getY());

    double theta = acos((vec2 * vec1)/(mag1 * mag2));
    double phi = asin(sin(theta)/ratio);

    double length = (mag2 * sin(phi))/(sin(theta + phi));

    return getPosition().closest(dimensions, vec1.normalized(length) + targetPos);
}

void Vessel::update(const Point &dimensions, double timeDiff) {
    double distance = getSpeed() * timeDiff;

    moveTowards(dimensions, getTargetPos(dimensions), distance);
}

// should be modified to work with specialist effects
double Vessel::getSpeed() const {
    const Player &p = *getOwner();

    return 1;
}

// generate collision events for other vessels
void Vessel::collision(const Point &dimensions, Vessel* other, time_t timestamp, std::multiset<Event*> &events) {
    if(getOwnerID() == other->getOwnerID() || getTargetID() != -1 || getOriginID() != -1) {
        return;
    }

    int seconds = -1;

    // Case 1: both are heading in the same direction, so it's a matter of whether the one behind can catch up
    if(getOriginID() == other->getOriginID() && getTargetID() == other->getTargetID()) {
        float speedDiff = getSpeed() - other->getSpeed();

        if(speedDiff != 0) {
            seconds = (distance(dimensions, target->getPosition()) - distance(dimensions, other->getPosition()))/speedDiff;
        }
    }
    // Case 2: both are heading towards each other, so they are guaranteed to collide
    else if((getOriginID() == other->getTargetID() && getTargetID() == other->getOriginID())
        || (getID() == other->getTargetID() && getTargetID() == other->getID())) {
        float speedSum = getSpeed() + other->getSpeed();

        if(speedSum > 0) {
            seconds = distance(dimensions, other->getPosition())/speedSum;
        }
    }
    // Case 3: this vessel is targeting the other
    else if(getID() == other->getTargetID()) {
        if(getSpeed() > 0) seconds = distance(dimensions, getTargetPos(dimensions))/getSpeed();
    }
    // Case 3: the other vessel is targeting this one
    else if(other->getID() == getTargetID()) {
        if(other->getSpeed() > 0) seconds = distance(dimensions, getTargetPos(dimensions))/other->getSpeed();
    }

    if(seconds > 0) {
        timestamp += seconds;
        events.insert(new IntervesselEvent(timestamp, this, other));

        return;
    }
}

// generate collision events for outposts
void Vessel::collision(const Point &dimensions, Outpost* outpost, time_t timestamp, std::multiset<Event*> &events) {
    int seconds = -1;

    if(getTargetID() == outpost->getID() && getTargetID() != -1) {
        if(getSpeed() > 0) seconds = int(distance(dimensions, outpost->getPosition())/getSpeed());
    }

    if(seconds > 0) {
        timestamp += seconds;
        events.insert(new VesselOutpostEvent(timestamp, this, outpost));

        return;
    }
}