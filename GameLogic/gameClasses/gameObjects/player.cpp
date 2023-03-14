#include "player.h"
#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "../events/win_condition_event.h"
#include "../game_settings.h"

void Player::updatePointers(Game* game) {
    for(std::shared_ptr<Specialist>& s : this->specialists) s = game->getSpecialist(s->getID());
    for(std::shared_ptr<Outpost>& o : this->outposts) o = game->getOutpost(o->getID());
    for(std::shared_ptr<Vessel>& v : this->vessels) v = game->getVessel(v->getID());
}

void Player::update(double timeDiff) {
    fractionalProduction += timeDiff * resourceProductionSpeed();
    while(fractionalProduction >= 1) {
        fractionalProduction -= 1;
        resources++;
    }
}

void Player::setDefeated(Game* game) {
    defeated = true;
    defeatedTime = game->getTime();

    for(const std::shared_ptr<Vessel>& v : vessels) v->setOwner(nullptr);
    for(const std::shared_ptr<Outpost>& o : outposts) o->setOwner(nullptr);
    for(const std::shared_ptr<Specialist>& s : specialists) s->setOwner(nullptr);

    vessels.clear();
    outposts.clear();
    specialists.clear();

    if(game->hasEnded()) game->endGame();
}

std::list<std::shared_ptr<Outpost>> Player::sortedOutposts(PositionalObject* obj) {
    std::list<std::shared_ptr<Outpost>> outposts;

    for(const std::shared_ptr<Outpost>& o : getOutposts()) {
        outposts.push_back(o);
    }

    outposts.sort([=](const std::shared_ptr<Outpost>& a, const std::shared_ptr<Outpost>& b) -> bool
    { 
        return obj->distance(Game::getDimensions(), a->getPosition()) < obj->distance(Game::getDimensions(), b->getPosition()); 
    });

    return outposts;
}

void Player::projectedVictory(std::shared_ptr<Player> player, time_t timestamp, std::multiset<std::shared_ptr<Event>, EventOrder> &events) {
    if(resourceProductionSpeed() > 0) {
        int diff = ceil((GameSettings::resourcesToWin - (getResources() + fractionalProduction)) / resourceProductionSpeed());
        events.insert(std::shared_ptr<Event>(new WinConditionEvent(timestamp + diff, player)));
    }
}

double Player::resourceProductionSpeed() {
    int outpostCount = 0;
    int mineCount = 0;

    for(std::shared_ptr<Outpost>& o : outposts) {
        if(o->getType() == OutpostType::MINE) mineCount++;
        outpostCount++;
    }

    return (mineCount * outpostCount) / (24.0 * 60.0 * 60.0);
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

bool Player::controlsSpecialist(SpecialistType t) const {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getContainer()->getOwnerID() == getID()) {
            return true;
        }
    }

    return false;
}

bool Player::controlsSpecialists(std::list<int> specialists) {
    int count = 0;

    for(std::shared_ptr<Specialist> specialist : this->specialists) {
        for(int id : specialists) {
            if(specialist->getID() == id && specialist->getContainer()->getOwnerID() == getID()){
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
    if(specialist->hasOwner()) specialist->getOwner()->removeSpecialist(specialist);

    specialist->setOwner(this);
    specialists.push_back(specialist);
    for(const std::shared_ptr<Vessel>& v : getVessels()) v->setRefresh(true);
}

void Player::addSpecialists(std::list<std::shared_ptr<Specialist>> specialists) {
    for(std::shared_ptr<Specialist>& specialist : specialists) addSpecialist(specialist);
}

void Player::removeSpecialist(std::shared_ptr<Specialist> specialist) {
    for(auto it = specialists.begin(); it != specialists.end(); it++) {
        if((*it)->getID() == specialist->getID()) {
            specialists.erase(it);
            break;
        }
    }

    specialist->setOwner(nullptr);
    for(const std::shared_ptr<Vessel>& v : getVessels()) v->setRefresh(true);
}

void Player::addOutpost(std::shared_ptr<Outpost> outpost) {
    if(outpost->hasOwner()) outpost->getOwner()->removeOutpost(outpost);

    outpost->setOwner(this);
    outpost->setRefresh(true);
    outposts.push_back(outpost);
}

void Player::removeOutpost(std::shared_ptr<Outpost> outpost) {
    for(auto it = outposts.begin(); it != outposts.end(); it++) {
        if((*it)->getID() == outpost->getID()) {
            outposts.erase(it);
            break;
        }
    }

    if(!outpost->getSpecialists().empty()) {
        for(const std::shared_ptr<Vessel>& v : getVessels()) v->setRefresh(true);
    }

    outpost->setOwner(nullptr);
}

void Player::addVessel(std::shared_ptr<Vessel> vessel) {
    if(vessel->hasOwner()) vessel->getOwner()->removeVessel(vessel);

    vessel->setOwner(this);
    vessel->setRefresh(true);
    vessels.push_back(vessel);
}

void Player::removeVessel(std::shared_ptr<Vessel> vessel) {
    for(auto it = vessels.begin(); it != vessels.end(); it++) {
        if((*it)->getID() == vessel->getID()) {
            vessels.erase(it);
            break;
        }
    }

    if(!vessel->getSpecialists().empty()) {
        for(const std::shared_ptr<Vessel>& v : getVessels()) v->setRefresh(true);
    }

    vessel->setOwner(nullptr);
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

    for(const std::shared_ptr<Outpost>& o : outposts) totalUnits += o->getUnits();

    for(const std::shared_ptr<Vessel>& v : vessels) totalUnits += v->getUnits();

    return totalUnits;
}