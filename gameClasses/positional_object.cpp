#include "positional_object.h"
#include "game.h"


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
    numUnits -= count;
    return count;
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

    return contains >= specialists.size();
}

std::list<std::shared_ptr<Specialist>> PositionalObject::removeSpecialists(std::list<std::shared_ptr<Specialist>> specialists) {
    for(auto it = this->specialists.begin(); it != this->specialists.end(); it++) {
        for(std::shared_ptr<Specialist> other : specialists) {
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