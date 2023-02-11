#include "positional_object.h"

double PositionalObject::distance(const Point &dimensions, const Point &other) const {
    return position.distance(position.closest(dimensions, other));
}

void PositionalObject::moveTowards(const Point &dimensions, const Point &other, double distance) {
    position += (position.closest(dimensions, other) - position).normalized(distance);

    position.constrain(dimensions);
}

bool PositionalObject::canRemoveUnits(int count) const {
    return numUnits >= count;
}

int PositionalObject::removeUnits(int count) {
    numUnits -= count;
    return count;
}

void PositionalObject::addSpecialists(std::list<Specialist*> specialists) {
    this->specialists.splice(this->specialists.end(), specialists);
}

bool PositionalObject::canRemoveSpecialists(std::list<int> specialistIDs) const {
    int contains = 0;
    for(Specialist* a : this->specialists) for(int id : specialistIDs) {
        if(a->getID() == id) {
            contains++;
            break;
        }
    }

    return contains >= specialists.size();
}

std::list<Specialist*> PositionalObject::removeSpecialists(std::list<Specialist*> specialists) {
    for(auto it = this->specialists.begin(); it != this->specialists.end(); it++) {
        for(Specialist* other : specialists) {
            if((*it)->getID() == other->getID()) {
                this->specialists.erase(it);
                break;
            }
        }
    }

    return specialists;
}

bool PositionalObject::hasSpecialist(SpecialistType t) const {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return true;
        }
    }

    return false;
}