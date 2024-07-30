#include "player.h"
#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "../events/win_condition_event.h"
#include "../game_settings.h"
#include <iostream>

void Player::updatePointers(Game* game) {
    for(Specialist* &s : this->specialists) s = game->getSpecialist(s->getID());
    for(Outpost* &o : this->outposts) o = game->getOutpost(o->getID());
    for(Vessel* &v : this->vessels) v = game->getVessel(v->getID());
}

int Player::getResourcesAt(double timeDiff) const {
    double fractionalResProduction = this->fractionalResProduction;
    
    return getResourcesAt(fractionalResProduction, timeDiff);
}

int Player::getHiresAt(double timeDiff) const {
    double fractionalHires = this->fractionalHires;

    return getHiresAt(fractionalHires, timeDiff);
}

/* The following two functions do not modify this instance unless you pass in
** references to the actual member variables, allowing them to be used for interpolation
** or updating the game state.
*/
int Player::getResourcesAt(double& fractionalResProduction, double timeDiff) const {
    timeDiff *= getSettings()->simulationSpeed;

    int resources = this->resources;

    fractionalResProduction += timeDiff * resourceProductionSpeed() * (1.0 / (24 * 60 * 60));
    resources += int(fractionalResProduction);
    fractionalResProduction -= int(fractionalResProduction);

    return std::max(resources, 0);
}

int Player::getHiresAt(double& fractionalHires, double timeDiff) const {
    timeDiff *= getSettings()->simulationSpeed;

    int hires = this->hires;

    fractionalHires += timeDiff * (1.0 / (24 * 60 * 60));
    hires += int(fractionalHires);
    fractionalHires -= int(fractionalHires);

    return std::max(hires, 0);
}

void Player::update(double timeDiff) {
    std::unordered_map<int, int> unitDiff = calculateUnitsAt(this->fractionalProduction, timeDiff);
    for(Outpost* o : outposts) if(unitDiff[o->getID()] > 0) o->addUnits(unitDiff[o->getID()]);
    for(Vessel* v : vessels) if(unitDiff[v->getID()] > 0) v->addUnits(unitDiff[v->getID()]);

    resources = getResourcesAt(this->fractionalResProduction, timeDiff);

    hires = getHiresAt(this->fractionalHires, timeDiff);
}

void Player::setDefeated(Game* game) {
    defeated = true;
    defeatedTime = game->getTime();

    setRefresh(true);

    for(Vessel* v : vessels) v->setOwner(nullptr);
    for(Outpost* o : outposts) o->setOwner(nullptr);
    for(Specialist* s : specialists) s->setOwner(nullptr);

    vessels.clear();
    outposts.clear();
    specialists.clear();

    if(game->hasEnded()) game->endGame();
}

PositionalObject* Player::getSpawnLocation() {
    for(Outpost* o : outposts) {
        if(o->controlsSpecialist(SpecialistType::QUEEN)) return o;
    }

    for(Vessel* v : vessels) {
        if(v->controlsSpecialist(SpecialistType::QUEEN)) return v;
    }

    for(Specialist* s : specialists) {
        if(s->getType() == QUEEN) return s->getContainer();
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

std::list<Vessel*> Player::sortedVessels(const PositionalObject* obj) {
    std::list<Vessel*> vessels;

    for(Vessel* v : getVessels()) vessels.push_back(v);

    vessels.sort([&obj](Vessel* a, Vessel* b) { 
        return obj->distance(a->getPosition()) < obj->distance(b->getPosition()); 
    });

    return vessels;
}

void Player::projectedVictory(Player* player, double timestamp, std::multiset<Event*, EventOrder> &events) {
    if(getSettings()->gameMode == Mode::MINING && resourceProductionSpeed() > 0) {
        int diff = ceil((getSettings()->resourcesToWin - (getResources() + fractionalProduction)) / (resourceProductionSpeed() / (24.0 * 60 * 60 / getSettings()->simulationSpeed)));
        if(diff < 0) return;
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

    return (mineCount * outpostCount);
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

    return std::max(0, amount);
}

double Player::globalProductionSpeed() const {
    double productionSpeed = 1;

    productionSpeed += 0.5 * expSpecialistEffect(SpecialistType::TYCOON);

    return productionSpeed;
}

double Player::globalSonar() const {
    double range = 1;

    range = 1 + 0.25 * expSpecialistEffect(SpecialistType::INTELLIGENCE_OFFICER);

    return range;
}


double Player::expSpecialistEffect(SpecialistType t) const {
    double effect = 0;
    double amount = 1.0;

    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t && (*it)->getContainer()->getOwnerID() == getID()) {
            effect += amount;
            amount *= 0.5;
        }
    }

    return effect;
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

void Player::promoteSpecialist(Specialist* specialist, SpecialistType t) {
    specialist->setType(t);
    for(Vessel* v : getVessels()) v->setRefresh(true);
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
    if(outpost->getType() == OutpostType::MINE) setRefresh(true);
    if(outpost->hasOwner()) outpost->getOwner()->removeOutpost(outpost);

    outpost->setOwner(this);
    outpost->setRefresh(true);
    outposts.push_back(outpost);
}

void Player::removeOutpost(Outpost* outpost) {
    for(Vessel* v : vessels) {
        if(v->controlsSpecialist(SpecialistType::SMUGGLER) && v->getTargetID() == outpost->getID()) v->setRefresh(true);
    }

    if(outpost->controlsSpecialist(SpecialistType::QUEEN)) {
        bool assigned = false;

        for(Outpost* o : sortedOutposts(outpost)) {
            if(o->controlsSpecialist(SpecialistType::PRINCESS)) {
                o->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                assigned = true;
                break;
            }
        }

        if(!assigned) {
            for(Vessel* v : sortedVessels(outpost)) {
                if(v->controlsSpecialist(SpecialistType::PRINCESS)) {
                    v->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                    break;
                }
            }
        }
    }

    for(auto it = outposts.begin(); it != outposts.end(); it++) {
        if((*it)->getID() == outpost->getID()) {
            if(outpost->getType() == OutpostType::MINE) {
                resources *= 1.0 - getSettings()->resourceReductionAmount;
                fractionalProduction = 0;
                setRefresh(true);
            }

            if(controlsSpecialist(SpecialistType::MINISTER_OF_WAR)) outpost->setType(OutpostType::BROKEN);

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
    if(vessel->controlsSpecialist(SpecialistType::QUEEN)) {
        bool assigned = false;

        for(Outpost* o : sortedOutposts(vessel)) {
            if(o->controlsSpecialist(SpecialistType::PRINCESS)) {
                o->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                assigned = true;
                break;
            }
        }

        if(!assigned) {
            for(Vessel* v : sortedVessels(vessel)) {
                if(v->controlsSpecialist(SpecialistType::PRINCESS)) {
                    v->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                    break;
                }
            }
        }
    }

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
    int totalCapacity = 150 + 50 * outpostsOfType(OutpostType::GENERATOR);

    for(Outpost* o : outposts) {
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

int Player::getUnitsAt(double timeDiff) const {
    int totalUnits = 0;

    for(Outpost* o : outposts) totalUnits += o->getUnitsAt(timeDiff);

    for(Vessel* v : vessels) totalUnits += v->getUnitsAt(timeDiff);

    return totalUnits;
}

int Player::dailyProductionRate() const {
    int totalProductionRate = 0;

    for(Outpost* o : outposts) {
        if(o->getProductionAmount() > 0 && o->getType() == OutpostType::FACTORY) {
            totalProductionRate += o->getProductionAmount();
        }
    }

    return totalProductionRate * globalProductionSpeed() * 3.0;
}

std::unordered_map<int, int> Player::calculateUnitsAt(double& fractionalProduction, double timeDiff) const {
    timeDiff *= getSettings()->simulationSpeed;

    std::unordered_map<int, int> units;

    int totalProductionRate = 0;

    std::list<PositionalObject*> tmp;
    for(Outpost* o : outposts) {
        if(o->getProductionAmount() > 0 && o->getType() == OutpostType::FACTORY) {
            tmp.push_back(o);
            totalProductionRate += o->getProductionAmount();
        }
        units[o->getID()] = 0;
    }

    for(Vessel* v : vessels) {
        if(v->getProductionAmount() > 0 && v->controlsSpecialist(SpecialistType::RECRUITER)) {
            tmp.push_back(v);
            totalProductionRate += v->getProductionAmount();
        }
        units[v->getID()] = 0;
    }

    if(timeDiff <= 0) return units;

    tmp.sort([]( const PositionalObject* a, const PositionalObject* b ) { return a->getID() < b->getID(); } );

    int totalUnits = getUnits();

    fractionalProduction += timeDiff * globalProductionSpeed() * (1.0 / (8.0 * 60 * 60));

    int productionCycles = int(fractionalProduction);

    int totalProduction = std::min(std::max(0, getCapacity() - totalUnits), productionCycles * totalProductionRate);

    int remaining = totalProduction;

    for(PositionalObject* o : tmp) {
        units[o->getID()] = int((o->getProductionAmount() * 1.0 / totalProductionRate) * totalProduction);
        remaining -= units[o->getID()];
    }

    while(remaining > 0) {
        for(PositionalObject* o : tmp) {
            units[o->getID()]++;
            remaining--;

            if(remaining <= 0) break;
        }
    }

    fractionalProduction -= productionCycles;

    return units;
}

int Player::outpostsOfType(OutpostType t) const {
    int count = 0;

    for(Outpost* o : outposts) {
        if(o->getType() == t) count++;
    }

    return count;
}

double Player::nextHireEvent(double timeDiff) const {
    int i = 1;
    double time;
    do {
        time = (i++ - getFractionalHires()) / (getSettings()->simulationSpeed / (24 * 60 * 60));
    } while(time < timeDiff);

    return time;
}
