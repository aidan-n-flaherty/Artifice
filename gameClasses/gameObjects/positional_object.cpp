#include "positional_object.h"
#include "../game.h"


void PositionalObject::updatePointers(Game* game) {
    for(std::shared_ptr<Specialist> &a : this->specialists) a = game->getSpecialist(a->getID());
}

double PositionalObject::distance(const Point &dimensions, const Point &other) const {
    return position.distance(position.closest(dimensions, other));
}

bool PositionalObject::canRemoveUnits(int count) const {
    return numUnits >= count;
}

int PositionalObject::removeUnits(int count) {
    if(count > numUnits) count = numUnits;
    numUnits -= count;
    return count;
}

void PositionalObject::addSpecialist(std::shared_ptr<Specialist> specialist) {
    specialists.push_back(specialist);
}

void PositionalObject::addSpecialists(std::list<std::shared_ptr<Specialist>> specialists) {
    this->specialists.splice(this->specialists.end(), specialists);
}

bool PositionalObject::canRemoveSpecialists(std::list<int> specialistIDs) const {
    int contains = 0;
    for(std::shared_ptr<Specialist> a : this->specialists) for(int id : specialistIDs) {
        if(a->getID() == id) {
            contains++;
            break;
        }
    }

    return contains >= specialistIDs.size();
}

std::list<std::shared_ptr<Specialist>> PositionalObject::removeSpecialists(std::list<std::shared_ptr<Specialist>> specialists) {
    for(auto it = this->specialists.begin(); it != this->specialists.end(); it++) {
        for(std::shared_ptr<Specialist> other : specialists) {
            if((*it)->getID() == other->getID()) {
                it = std::prev(this->specialists.erase(it));
                break;
            }
        }
    }

    return specialists;
}

int PositionalObject::specialistCount(SpecialistType t) const {
    int count = 0;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getOwnerID() == getOwnerID()) count++;
    }

    return count;
}

bool PositionalObject::hasSpecialist(SpecialistType t) const {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getOwnerID() == getOwnerID()) {
            return true;
        }
    }

    return false;
}

std::shared_ptr<Specialist> PositionalObject::getSpecialist(SpecialistType t) {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return *it;
        }
    }

    return nullptr;
}

void PositionalObject::specialistPhase(int &units, int& otherUnits, std::shared_ptr<PositionalObject> other) {
    otherUnits -= 5 * specialistCount(SpecialistType::LIEUTENANT);

    units += int(0.15 * specialistCount(SpecialistType::THIEF) * other->getUnits());
    otherUnits -= int(0.15 * specialistCount(SpecialistType::THIEF) * other->getUnits());

    if(!getSpecialists().empty()) otherUnits -= 10 * getOwner()->specialistCount(SpecialistType::GENERAL);

    otherUnits -= 20 * specialistCount(SpecialistType::WAR_HERO);
}


void PositionalObject::postSpecialistPhase(int& units, int& otherUnits, std::shared_ptr<PositionalObject> other) {
    if(hasSpecialist(SpecialistType::KING)) otherUnits -= int(getUnits()/3.0);
}

void PositionalObject::victorySpecialistPhase(int lostUnits, int otherLostUnits, std::shared_ptr<PositionalObject> other) {
    if(hasSpecialist(SpecialistType::ENGINEER)) addUnits(int(0.25 * lostUnits));
    if(getOwner()->hasSpecialist(SpecialistType::ENGINEER)) addUnits(int(0.25 * lostUnits));
}

void PositionalObject::defeatSpecialistPhase(int lostUnits, int otherLostUnits, std::shared_ptr<PositionalObject> other) {

}