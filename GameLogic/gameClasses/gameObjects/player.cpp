#include "player.h"
#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "../events/win_condition_event.h"
#include "../game_settings.h"

void Player::updatePointers(Game* game) {
    for(Specialist* &s : this->specialists) s = game->getSpecialist(s->getID());
    for(Outpost* &o : this->outposts) o = game->getOutpost(o->getID());
    for(Vessel* &v : this->vessels) v = game->getVessel(v->getID());
}

int Player::getResourcesAt(double timeDiff) const {
    double fractionalProduction = this->fractionalProduction;
    
    return getResourcesAt(fractionalProduction, timeDiff);
}

int Player::getHiresAt(double timeDiff) const {
    double fractionalHires = this->fractionalHires;

    return getHiresAt(fractionalHires, timeDiff);
}

/* The following two functions do not modify this instance unless you pass in
** references to the actual member variables, allowing them to be used for interpolation
** or updating the game state.
*/
int Player::getResourcesAt(double& fractionalProduction, double timeDiff) const {
    timeDiff *= GameSettings::simulationSpeed;

    int resources = this->resources;

    // while loops necessary in case a tick doesn't happen for several hours
    fractionalProduction += timeDiff * resourceProductionSpeed();
    while(fractionalProduction >= 1) {
        fractionalProduction -= 1;
        resources += 1;
    }

    return resources;
}

int Player::getHiresAt(double& fractionalHires, double timeDiff) const {
    timeDiff *= GameSettings::simulationSpeed;

    int hires = this->hires;

    fractionalHires += timeDiff * (1.0 / (24 * 60 * 60));
    while(fractionalHires >= 1) {
        fractionalHires -= 1;
        hires += 1;
    }

    return hires;
}

void Player::update(double timeDiff) {
    resources = getResourcesAt(this->fractionalProduction, timeDiff);

    hires = getHiresAt(this->fractionalHires, timeDiff);
}

void Player::setDefeated(Game* game) {
    defeated = true;
    defeatedTime = game->getTime();

    for(Vessel* v : vessels) v->setOwner(nullptr);
    for(Outpost* o : outposts) o->setOwner(nullptr);
    for(Specialist* s : specialists) s->setOwner(nullptr);

    vessels.clear();
    outposts.clear();
    specialists.clear();

    if(game->hasEnded()) game->endGame();
}

PositionalObject* Player::getSpawnLocation() {
    for(Specialist* s : specialists) {
        if(s->getType() == QUEEN) {
            return s->getContainer();
        }
    }

    return nullptr;
}

std::list<Outpost*> Player::sortedOutposts(const PositionalObject* obj) {
    std::list<Outpost*> outposts;

    for(Outpost* o : getOutposts()) outposts.push_back(o);

    outposts.sort([&obj](Outpost* a, Outpost* b) { 
        return obj->distance(a->getPosition()) < obj->distance(b->getPosition()); 
    });

    return outposts;
}

void Player::projectedVictory(Player* player, double timestamp, std::multiset<Event*, EventOrder> &events) {
    if(resourceProductionSpeed() > 0) {
        int diff = ceil((GameSettings::resourcesToWin - (getResources() + fractionalProduction)) / resourceProductionSpeed());
        events.insert(new WinConditionEvent(timestamp + diff, player));
    }
}

double Player::resourceProductionSpeed() const {
    int outpostCount = 0;
    int mineCount = 0;

    for(Outpost* o : outposts) {
        if(o->getType() == OutpostType::MINE) mineCount++;
        outpostCount++;
    }

    return (mineCount * outpostCount) / (24.0 * 60.0 * 60.0);
}

double Player::globalSpeed() const {
    return 1;
}

int Player::globalMaxShield() const {
    int maxShield = 0;

    maxShield += 10 * specialistCount(SpecialistType::SECURITY_CHIEF);

    maxShield -= 20 * specialistCount(SpecialistType::KING);

    return maxShield;
}

int Player::globalProductionAmount() const {
    int amount = 6;

    amount -= specialistCount(SpecialistType::MINISTER_OF_ENERGY);

    return amount;
}

double Player::globalProductionSpeed() const {
    double productionSpeed = 1;

    productionSpeed += 0.5 * specialistCount(SpecialistType::TYCOON);

    return productionSpeed;
}

double Player::globalSonar() const {
    double range = 1;

    range = 1 + 0.25 * specialistCount(SpecialistType::INTELLIGENCE_OFFICER);

    return range;
}

int Player::specialistCount(SpecialistType t) const {
    int count = 0;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getContainer()->getOwnerID() == getID()) count++;
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

bool Player::controlsSpecialists(std::list<int> specialists) const {
    int count = 0;

    for(Specialist* specialist : this->specialists) {
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

void Player::addSpecialist(Specialist* specialist) {
    if(specialist->hasOwner()) specialist->getOwner()->removeSpecialist(specialist);
    if(controlsSpecialist(SpecialistType::QUEEN) && specialist->getType() == SpecialistType::QUEEN) specialist->setType(SpecialistType::PRINCESS);
    
    specialist->setOwner(this);
    specialists.push_back(specialist);
    for(Vessel* v : getVessels()) v->setRefresh(true);
}

void Player::addSpecialists(std::list<Specialist*> specialists) {
    for(Specialist* specialist : specialists) addSpecialist(specialist);
}

void Player::removeSpecialist(Specialist* specialist) {
    for(auto it = specialists.begin(); it != specialists.end(); it++) {
        if((*it)->getID() == specialist->getID()) {
            specialists.erase(it);
            break;
        }
    }

    specialist->setOwner(nullptr);
    for(Vessel* v : getVessels()) v->setRefresh(true);
}

void Player::addOutpost(Outpost* outpost) {
    if(outpost->hasOwner()) outpost->getOwner()->removeOutpost(outpost);

    outpost->setOwner(this);
    outpost->setRefresh(true);
    outposts.push_back(outpost);
}

void Player::removeOutpost(Outpost* outpost) {
    for(auto it = outposts.begin(); it != outposts.end(); it++) {
        if((*it)->getID() == outpost->getID()) {
            outposts.erase(it);
            break;
        }
    }

    if(!outpost->getSpecialists().empty()) {
        for(Vessel* v : getVessels()) v->setRefresh(true);
    }

    outpost->setOwner(nullptr);
}

void Player::addVessel(Vessel* vessel) {
    if(vessel->hasOwner()) vessel->getOwner()->removeVessel(vessel);

    vessel->setOwner(this);
    vessel->setRefresh(true);
    vessels.push_back(vessel);
}

void Player::removeVessel(Vessel* vessel) {
    for(auto it = vessels.begin(); it != vessels.end(); it++) {
        if((*it)->getID() == vessel->getID()) {
            vessels.erase(it);
            break;
        }
    }

    if(!vessel->getSpecialists().empty()) {
        for(Vessel* v : getVessels()) v->setRefresh(true);
    }

    vessel->setOwner(nullptr);
}

int Player::getCapacity() const {
    int totalCapacity = 150;

    for(Outpost* o : outposts) {
        if(o->getType() == OutpostType::GENERATOR) totalCapacity += 50;

        totalCapacity += o->specialistCount(SpecialistType::TINKERER) * 3 * o->getMaxShield();
    }

    totalCapacity += 300 * specialistCount(SpecialistType::MINISTER_OF_ENERGY);

    return totalCapacity;
}

int Player::getUnits() const {
    int totalUnits = 0;

    for(Outpost* o : outposts) totalUnits += o->getUnits();

    for(Vessel* v : vessels) totalUnits += v->getUnits();

    return totalUnits;
}