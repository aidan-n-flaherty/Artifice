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

bool PositionalObject::controlsSpecialist(SpecialistType t) const {
    if(!hasOwner()) return false;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getOwnerID() == getOwnerID()) {
            return true;
        }
    }

    return false;
}

Specialist* PositionalObject::getSpecialist(SpecialistType t) {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return *it;
        }
    }

    return nullptr;
}

void PositionalObject::specialistPhase(int &units, int& otherUnits, PositionalObject* other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    otherUnits -= 5 * specialistCount(SpecialistType::LIEUTENANT);

    units += int(0.15 * specialistCount(SpecialistType::THIEF) * other->getUnits());
    otherUnits -= int(0.15 * specialistCount(SpecialistType::THIEF) * other->getUnits());

    if(!getSpecialists().empty()) otherUnits -= 10 * ownerSpecialistCount(SpecialistType::GENERAL);

    otherUnits -= 20 * specialistCount(SpecialistType::WAR_HERO);
}


void PositionalObject::postSpecialistPhase(int& units, int& otherUnits, PositionalObject* other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(ownerControlsSpecialist(SpecialistType::KING)) otherUnits -= int(units/3.0);
}

void PositionalObject::victorySpecialistPhase(int lostUnits, int otherLostUnits, PositionalObject* other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(controlsSpecialist(SpecialistType::ENGINEER)) addUnits(int(0.25 * lostUnits));
    if(ownerControlsSpecialist(SpecialistType::ENGINEER)) addUnits(int(0.25 * lostUnits));
}

void PositionalObject::defeatSpecialistPhase(int lostUnits, int otherLostUnits, PositionalObject* other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;
}

void PositionalObject::postCombatSpecialistPhase(Game* game, PositionalObject* other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(controlsSpecialist(SpecialistType::MARTYR)) {
        int range = GameSettings::defaultSonar/4;

        for(const auto &v : game->getVessels()) {
            if(distance(v.second->getPosition()) <= range) game->removeVessel(v.second);
        }

        for(const auto &o : game->getOutposts()) {
            if(distance(o.second->getPosition()) <= range) {
                o.second->setType(OutpostType::BROKEN);
                o.second->setUnits(0);

                while(!o.second->getSpecialists().empty()) game->removeSpecialist(o.second->getSpecialists().front());
            }
        }

        game->removeVessel(game->getVessel(getID()));
    }

    // to ensure order invariance, we have to check if both vessels have assassins first
    if(controlsSpecialist(SpecialistType::ASSASSIN) && other->controlsSpecialist(SpecialistType::ASSASSIN)) {
        while(!other->getSpecialists().empty()) game->removeSpecialist(other->getSpecialists().front());
        while(!getSpecialists().empty()) game->removeSpecialist(getSpecialists().front());
    } else if(controlsSpecialist(SpecialistType::ASSASSIN)) {
        while(!other->getSpecialists().empty()) game->removeSpecialist(other->getSpecialists().front());
    }
}