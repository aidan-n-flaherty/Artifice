#include "player.h"
#include "gameObjects/specialist.h"
#include "gameObjects/outpost.h"
#include "game.h"

void Player::updatePointers(Game* game) {
    for(std::shared_ptr<Specialist>& s : this->specialists) s = game->getSpecialist(s->getID());
    for(std::shared_ptr<Outpost>& o : this->outposts) o = game->getOutpost(o->getID());
}

void Player::update(double timeDiff) {
    int outpostCount = 0;
    int mineCount = 0;

    for(std::shared_ptr<Outpost>& o : outposts) {
        if(o->getType() == OutpostType::MINE) mineCount++;
        outpostCount++;
    }

    fractionalProduction += timeDiff * (mineCount * outpostCount / (24.0 * 60.0 * 60.0));
    while(fractionalProduction >= 1) {
        fractionalProduction -= 1;
        resources++;
    }
}

double Player::globalSpeed(){
    return 1;
}

int Player::globalMaxShield() {
    int maxShield = 0;

    maxShield += 10 * specialistCount(SpecialistType::SECURITY_CHIEF);

    maxShield -= 20 * specialistCount(SpecialistType::KING);

    return maxShield;
}

int Player::globalProductionAmount() {
    int amount = 6;

    amount -= specialistCount(SpecialistType::MINISTER_OF_ENERGY);

    return amount;
}

double Player::globalProductionSpeed() {
    double productionSpeed = 1;

    productionSpeed += 0.5 * specialistCount(SpecialistType::TYCOON);

    return productionSpeed;
}

double Player::globalSonar() {
    double range = 1;

    range = 1 + 0.25 * specialistCount(SpecialistType::INTELLIGENCE_OFFICER);

    return range;
}

int Player::specialistCount(SpecialistType t) const {
    int count = 0;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) count++;
    }

    return count;
}

bool Player::hasSpecialist(SpecialistType t) const {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return true;
        }
    }

    return false;
}

bool Player::hasSpecialists(std::list<int> specialists) {
    int count = 0;

    for(std::shared_ptr<Specialist> specialist : this->specialists) {
        for(int id : specialists) {
            if(specialist->getID() == id){
                count++;
                
                if(count == specialists.size()) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Player::addSpecialist(std::shared_ptr<Specialist> specialist) {
    specialists.push_back(specialist);
}

void Player::removeSpecialist(std::shared_ptr<Specialist> specialist) {
    for(auto it = specialists.begin(); it != specialists.end(); it++) {
        if((*it)->getID() == specialist->getID()) {
            specialists.erase(it);
            break;
        }
    }
}

int Player::getCapacity() const {
    int totalCapacity = 150;

    for(const std::shared_ptr<Outpost>& o : outposts) {
        if(o->getType() == OutpostType::GENERATOR) totalCapacity += 50;

        totalCapacity += o->specialistCount(SpecialistType::TINKERER) * 3 * o->getMaxShield();
    }

    totalCapacity += 300 * specialistCount(SpecialistType::MINISTER_OF_ENERGY);

    return totalCapacity;
}

int Player::getUnits() const {
    int totalUnits = 0;

    for(const std::shared_ptr<Outpost>& o : outposts) {
        totalUnits += o->getUnits();
    }

    for(const std::shared_ptr<Vessel>& v : vessels) {
        totalUnits += v->getUnits();
    }

    return totalUnits;
}