#include "positional_object.h"
#include "../game.h"
#include "../game_settings.h"

void PositionalObject::updatePointers(Game* game) {
    Possessable::updatePointers(game);
    for(Specialist* &a : this->specialists) a = game->getSpecialist(a->getID());
}

double PositionalObject::distance(const Point &other) const {
    return position.distance(position.closest(other));
}

bool PositionalObject::canRemoveUnits(int count) const {
    return numUnits >= count;
}

int PositionalObject::removeUnits(int count) {
    if(count > numUnits) count = numUnits;
    numUnits -= count;
    return count;
}

void PositionalObject::addSpecialist(Specialist* specialist) {
    specialists.push_back(specialist);
    specialist->setContainer(this);
}

void PositionalObject::addSpecialists(std::list<Specialist*> specialists) {
    for(Specialist* specialist : specialists) addSpecialist(specialist);
}

bool PositionalObject::canRemoveSpecialists(std::list<int> specialistIDs) const {
    int contains = 0;
    for(Specialist* a : this->specialists) for(int id : specialistIDs) {
        if(a->getID() == id) {
            contains++;
            break;
        }
    }

    return contains >= specialistIDs.size();
}

std::list<Specialist*> PositionalObject::removeSpecialists() {
    std::list<Specialist*> tmp = specialists;
    specialists.clear();

    return tmp;
}

std::list<Specialist*> PositionalObject::removeSpecialists(std::list<Specialist*> specialists) {
    for(auto it = this->specialists.begin(); it != this->specialists.end(); it++) {
        for(Specialist* other : specialists) {
            if((*it)->getID() == other->getID()) {
                it = std::prev(this->specialists.erase(it));
                break;
            }
        }
    }

    return specialists;
}

Specialist* PositionalObject::removeSpecialist(Specialist* specialist) {
    for(auto it = this->specialists.begin(); it != this->specialists.end(); it++) {
        if((*it)->getID() == specialist->getID()) {
            this->specialists.erase(it);
            return specialist;
        }
    }

    return nullptr;
}


bool PositionalObject::ownerControlsSpecialist(SpecialistType type) const {
    return hasOwner() && getOwner()->controlsSpecialist(type);
}

int PositionalObject::ownerSpecialistCount(SpecialistType type) const {
    return hasOwner() ? getOwner()->specialistCount(type) : 0;
}

int PositionalObject::specialistCount(SpecialistType t) const {
    if(!hasOwner()) return 0;

    int count = 0;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getOwnerID() == getOwnerID()) count++;
    }

    return count;
}

bool PositionalObject::controlsSpecialist(Player* p, std::list<Specialist*> specialists, SpecialistType t) {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getOwnerID() == p->getID()) {
            return true;
        }
    }

    return false;
}

bool PositionalObject::controlsSpecialist(SpecialistType t) const {
    if(!hasOwner()) return false;

    return PositionalObject::controlsSpecialist(getOwner(), specialists, t);
}

Specialist* PositionalObject::getSpecialist(SpecialistType t) {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return *it;
        }
    }

    return nullptr;
}