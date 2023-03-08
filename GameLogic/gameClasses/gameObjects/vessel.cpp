#include "vessel.h"
#include "specialist.h"
#include "../events/intervessel_event.h"
#include "../events/vessel_outpost_event.h"
#include <set>
#include <iostream>
#include <cmath>

void Vessel::updatePointers(Game* game) {
    PositionalObject::updatePointers(game);
    Possessable::updatePointers(game);

    origin = game->getOutpost(origin->getID());
    target = game->getPosObject(target->getID());
}

/* Calculates the point that this vessel is currently targeting, taking the target's
** movement into account.
** Uses law of sines to derive answer.
*/
const Point Vessel::getTargetPos(const Point &dimensions) const {
    // return the target position if the target does not move
    Point targetPos = getPosition().closest(dimensions, target->getPosition());
    if(getSpeed() == 0 || target->getSpeed() == 0) return targetPos;

    Point returnVal;
    Point targetTarget = targetPos.closest(dimensions, target->getTargetPos(dimensions));

    /* Construct a triangle with sides A: x, B: yx, and C: z, where y is the ratio between
    ** sides B and A, and z is the current distance between the vessels. We know the angle 
    ** between A and C.
    ** x is the unknown, and determines the point of intersection.
    */

    double ratio = getSpeed()/target->getSpeed();

    // loop twice in the event that it would be faster to move in the opposite direction
    for(int i = 0; i < 2; i++) {
        Point vec1(targetTarget.getX() - targetPos.getX(), targetTarget.getY() - targetPos.getY());
        double mag1 = sqrt(vec1.getX() * vec1.getX() + vec1.getY() * vec1.getY());

        Point vec2(getPosition().getX() - targetPos.getX(), getPosition().getY() - targetPos.getY());
        double mag2 = sqrt(vec2.getX() * vec2.getX() + vec2.getY() * vec2.getY());

        double cosTheta = (vec2 * vec1)/(mag1 * mag2);
        double a = (ratio * ratio - 1);
        double b = 2 * mag2 * cosTheta;
        double c = -mag2 * mag2;

        double lambda = b * b - 4 * a * c;
        if(lambda < 0) continue;
        double length = a == 0 && b == 0 ? -1 : a == 0 ? -c/b : (-b + sqrt(lambda))/(2 * a);

        returnVal = vec1.normalized(length) + targetPos;

        // there's a shorter path if the projected arrival happens outside the wrap-around box
        if(returnVal == getPosition().closest(dimensions, returnVal) && length <= mag1 && length >= 0) break;
        // if both paths fail, then return to origin
        else if(i == 1) return Point();

        targetTarget = getPosition().closest(dimensions, target->getTargetPos(dimensions));
        targetPos = targetTarget.closest(dimensions, target->getPosition());
    }

    return returnVal;
}

const Point Vessel::getPositionAt(const Point &dimensions, double timeDiff) const {
    double distance = getSpeed() * timeDiff;

    Point target = getTargetPos(dimensions);
    if(target.isInvalid()) {
        target = getPosition().closest(dimensions, origin->getPosition());
    }

    return getPosition().movedTowards(dimensions, target, distance);
}

void Vessel::update(const Point &dimensions, double timeDiff) {
    if(target->isDeleted()) setTarget(origin);
    double distance = getSpeed() * timeDiff;

    Point targetedPos = getTargetPos(dimensions);
    if(targetedPos.isInvalid()) {
        setTarget(origin);
        targetedPos = getTargetPos(dimensions);
    }

    moveTowards(dimensions, targetedPos, distance);
}

// should be modified to work with specialist effects
double Vessel::getSpeed() const {
    double speed = 1;

    if(controlsSpecialist(SpecialistType::GENERAL) || controlsSpecialist(SpecialistType::LIEUTENANT)) speed = fmax(speed, 1.5);
    if(getSpecialists().empty() && getOwner()->controlsSpecialist(SpecialistType::ADMIRAL)) speed = fmax(speed, 1.5);
    if(controlsSpecialist(SpecialistType::ADMIRAL)) speed = fmax(speed, 2);
    if(controlsSpecialist(SpecialistType::SMUGGLER) && getTarget()->getOwnerID() == getOwnerID()) speed = fmax(speed, 3);

    return speed;
}

// generate collision events for other vessels
void Vessel::collision(const Point &dimensions, std::shared_ptr<Vessel> vessel, std::shared_ptr<Vessel> other, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events) {
    if(vessel->getOwnerID() == other->getOwnerID() || vessel->getTargetID() == -1 || vessel->getOriginID() == -1) return;

    int seconds = -1;
    // Case 1: both are heading in the same direction, so it's a matter of whether the one behind can catch up
    if(vessel->getOriginID() == other->getOriginID() && vessel->getTargetID() == other->getTargetID()) {
        float speedDiff = vessel->getSpeed() - other->getSpeed();

        if(speedDiff != 0) {
            seconds = (vessel->distance(dimensions, target->getPosition()) - vessel->distance(dimensions, other->getPosition()))/speedDiff;
        }
    }
    // Case 2: both are heading towards each other, so they are guaranteed to collide
    else if((vessel->getOriginID() == other->getTargetID() && vessel->getTargetID() == other->getOriginID())
        || (vessel->getID() == other->getTargetID() && vessel->getTargetID() == other->getID())) {
        float speedSum = vessel->getSpeed() + other->getSpeed();

        if(speedSum > 0) seconds = vessel->distance(dimensions, other->getPosition())/speedSum;
    }
    // Case 3: this vessel is targeting the other
    else if(vessel->getTargetID() == other->getID()) {
        if(vessel->getSpeed() > 0) seconds = vessel->distance(dimensions, vessel->getTargetPos(dimensions))/vessel->getSpeed();
    }
    // Case 3: the other vessel is targeting this one
    else if(other->getTargetID() == vessel->getID()) {
        if(other->getSpeed() > 0) seconds = vessel->distance(dimensions, other->getTargetPos(dimensions))/other->getSpeed();
    }

    if(seconds >= 0) {
        timestamp += seconds;
        events.insert(std::shared_ptr<Event>(new IntervesselEvent(timestamp, vessel, other)));

        return;
    }
}

// generate collision events for outposts
void Vessel::collision(const Point &dimensions, std::shared_ptr<Vessel> vessel, std::shared_ptr<Outpost> outpost, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events) {
    int seconds = -1;

    if(vessel->getTargetID() == outpost->getID() && vessel->getTargetID() != -1) {
        if(vessel->getSpeed() > 0) seconds = int(vessel->distance(dimensions, outpost->getPosition())/vessel->getSpeed());
    }

    if(seconds >= 0) {
        timestamp += seconds;
        events.insert(std::shared_ptr<Event>(new VesselOutpostEvent(timestamp, vessel, outpost)));

        return;
    }
}

void Vessel::specialistPhase(int& units, int& otherUnits, std::shared_ptr<Vessel> other) {
    // check if only one side has a revered elder
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) != other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    PositionalObject::specialistPhase(units, otherUnits, other);

    if(controlsSpecialist(SpecialistType::SABOTEUR)) other->setTarget(other->getOrigin());

    if(controlsSpecialist(SpecialistType::DOUBLE_AGENT)) {
        // change ownership of all specialists
        for(std::shared_ptr<Specialist> specialist : getSpecialists()) {
            getOwner()->removeSpecialist(specialist);
            other->getOwner()->addSpecialist(specialist);
        }

        for(std::shared_ptr<Specialist> specialist : other->getSpecialists()) {
            getOwner()->removeSpecialist(specialist);
            other->getOwner()->addSpecialist(specialist);
        }

        units = 0;
        otherUnits = 0;

        // swap owners
        std::shared_ptr<Player> otherOwner = other->getOwner();
        other->setOwner(getOwner());
        setOwner(otherOwner);
    }
}

void Vessel::specialistPhase(int& units, int& otherUnits, std::shared_ptr<Outpost> other) {
    // check if only one side has a revered elder
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) != other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    PositionalObject::specialistPhase(units, otherUnits, other);

    if(controlsSpecialist(SpecialistType::INFILTRATOR)) other->removeShield(other->getShield());
}