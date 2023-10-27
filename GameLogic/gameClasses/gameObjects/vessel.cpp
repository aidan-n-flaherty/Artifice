#include "vessel.h"
#include "specialist.h"
#include "../events/intervessel_event.h"
#include "../events/vessel_outpost_event.h"
#include "../game_settings.h"
#include <set>
#include <iostream>
#include <cmath>

void Vessel::updatePointers(Game* game) {
    PositionalObject::updatePointers(game);

    returnOutpost = game->getOutpost(returnOutpost->getID());
    origin = game->getOutpost(getOriginID());
    target = game->getPosObject(target->getID());
}

/* Calculates the point that this vessel is currently targeting, taking the target's
** movement into account.
*/
const Point Vessel::getTargetPos() const {
    // return the target position if the target does not move
    Point targetPos = getPosition().closest(target->getPosition());
    if(getSpeed() == 0 || target->getSpeed() == 0) return targetPos;

    Point returnVal;
    Point targetTarget = targetPos.closest(target->getTargetPos());

    // Does some fancy vector math to find the intersection point

    // loop twice in the event that it would be faster to move in the opposite direction
    for(int i = 0; i < 2; i++) {
        Point targetDelta = Point(targetTarget.getX() - targetPos.getX(), targetTarget.getY() - targetPos.getY()).normalized(target->getSpeed());
        Point diff = targetPos - getPosition();

        double a = (targetDelta * targetDelta - getSpeed() * getSpeed());
        double b = diff * 2 * targetDelta;
        double c = diff * diff;

        double lambda = b * b - 4 * a * c;
        if(lambda < 0) continue;
        double t = a == 0 && b == 0 ? -1 : a == 0 ? -c/b : std::max((-b + sqrt(lambda))/(2 * a), (-b - sqrt(lambda))/(2 * a));
        if(t < 0) continue;

        returnVal = targetPos + targetDelta * t;

        // there's a shorter path if the projected arrival happens outside the wrap-around box
        if(returnVal == getPosition().closest(returnVal)) break;
        // if both paths fail, then return to origin
        else if(i == 1) return Point();

        targetTarget = getPosition().closest(target->getTargetPos());
        targetPos = targetTarget.closest(target->getPosition());
    }

    return returnVal;
}

const Point Vessel::getPositionAt(double timeDiff) const {
    double distance = getSpeed() * timeDiff;

    return getPosition().movedTowards(getTargetPos(), distance);
}

void Vessel::update(double timeDiff) {
    double distance = getSpeed() * timeDiff;

    Point targetedPos = getTargetPos();
    if(targetedPos.isInvalid()) {
        returnHome();
        targetedPos = getTargetPos();
    }

    moveTowards(targetedPos, distance);
}

double Vessel::getSpeed(double speed, double simulationSpeed, Player* p, std::list<Specialist*> specialists, PositionalObject* target) {
    if(controlsSpecialist(p, specialists, SpecialistType::GENERAL) || controlsSpecialist(p, specialists, SpecialistType::LIEUTENANT)) speed = fmax(speed, 1.5);
    if(specialists.empty() && p->controlsSpecialist(SpecialistType::ADMIRAL)) speed = fmax(speed, 1.5);
    if(controlsSpecialist(p, specialists, SpecialistType::ADMIRAL)) speed = fmax(speed, 2);
    if(controlsSpecialist(p, specialists, SpecialistType::HELMSMAN)) speed = fmax(speed, 2);
    if(controlsSpecialist(p, specialists, SpecialistType::PIRATE) && (!target || dynamic_cast<Vessel*>(target))) speed = fmax(speed, 2);
    if(controlsSpecialist(p, specialists, SpecialistType::SMUGGLER) && (!target || target->getOwnerID() == p->getID())) speed = fmax(speed, 3);

    return speed * (simulationSpeed * 2.0 / (60 * 60));
}

// should be modified to work with specialist effects
double Vessel::getSpeed() const {
    double speed = speedModifier;

    return Vessel::getSpeed(speed, getSettings()->simulationSpeed, getOwner(), getSpecialists(), getTarget());
}

// generate collision events for other vessels
void Vessel::collision(Vessel* vessel, Vessel* other, double timestamp, std::multiset<Event*, EventOrder> &events) {
    if(vessel->getOwnerID() == other->getOwnerID() || vessel->getTargetID() == -1) return;

    double seconds = -1;
    // Case 1: both are heading in the same direction, so it's a matter of whether the one behind can catch up
    if(vessel->getOriginID() == other->getOriginID() && vessel->getTargetID() == other->getTargetID()) {
        float speedDiff = vessel->getSpeed() - other->getSpeed();

        if(speedDiff != 0) {
            seconds = (vessel->distance(target->getPosition()) - vessel->distance(other->getPosition()))/speedDiff;
        }
    }
    // Case 2: both are heading towards each other, so they are guaranteed to collide
    else if((vessel->getOriginID() == other->getTargetID() && vessel->getTargetID() == other->getOriginID())
        || (vessel->getID() == other->getTargetID() && vessel->getTargetID() == other->getID())) {
        float speedSum = vessel->getSpeed() + other->getSpeed();

        if(speedSum > 0) seconds = vessel->distance(other->getPosition())/speedSum;
    }
    // Case 3: this vessel is targeting the other
    else if(vessel->getTargetID() == other->getID()) {
        if(vessel->getSpeed() > 0) seconds = vessel->distance(vessel->getTargetPos())/vessel->getSpeed();
    }
    // Case 3: the other vessel is targeting this one
    else if(other->getTargetID() == vessel->getID()) {
        if(other->getSpeed() > 0) seconds = vessel->distance(other->getTargetPos())/other->getSpeed();
    }

    if(seconds >= 0) {
        timestamp += seconds;
        events.insert(new IntervesselEvent(timestamp, vessel, other));

        return;
    }
}

// generate collision events for outposts
void Vessel::collision(Vessel* vessel, Outpost* outpost, double timestamp, std::multiset<Event*, EventOrder> &events) {
    double seconds = -1;

    if(vessel->getTargetID() == outpost->getID() && vessel->getTargetID() != -1) {
        if(vessel->getSpeed() > 0) seconds = vessel->distance(outpost->getPosition())/vessel->getSpeed();
    }

    if(seconds >= 0) {
        timestamp += seconds;
        events.insert(new VesselOutpostEvent(timestamp, vessel, outpost));

        return;
    }
}

void Vessel::returnHome() {
    if(hasOwner() && !getOwner()->getOutposts().empty()) setTarget(getOwner()->sortedOutposts(this).front());
    else setTarget(returnOutpost);
}