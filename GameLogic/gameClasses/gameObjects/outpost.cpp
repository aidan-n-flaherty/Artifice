#include "vessel.h"
#include <list>
#include <cmath>
#include <algorithm>
#include "player.h"
#include "../game_settings.h"

int Outpost::getUnitsAt(double timeDiff) const {
    double fractionalProduction = hasOwner() ? getOwner()->getFractionalProduction() : 0;
    
    return getUnitsAt(fractionalProduction, timeDiff);
}

int Outpost::getShieldAt(double timeDiff) const {
    double fractionalShield = this->fractionalShield;

    return getShieldAt(fractionalShield, timeDiff);
}

double Outpost::nextProductionEvent(double timeDiff) const {
    if(!hasOwner()) return -1;

    int i = 1;
    double time;
    do {
        time = (i++ - getOwner()->getFractionalProduction()) / (getOwner()->globalProductionSpeed() * getSettings()->simulationSpeed / (8.0 * 60 * 60));
    } while(time < timeDiff);

    return time;
}

/* The following two functions do not modify this instance unless you pass in
** references to the actual member variables, allowing them to be used for interpolation
** or updating the game state.
*/
int Outpost::getUnitsAt(double& fractionalProduction, double timeDiff) const {
    int units = getUnits();

    if(!hasOwner() || type != OutpostType::FACTORY) return units;

    return units + getOwner()->calculateUnitsAt(fractionalProduction, timeDiff)[getID()];
}

int Outpost::getShieldAt(double& fractionalShield, double timeDiff) const {
    if(timeDiff <= 0) timeDiff = 0;
    
    timeDiff *= getSettings()->simulationSpeed;

    double shieldCharge = this->shieldCharge;

    if(!hasOwner()) return shieldCharge;

    fractionalShield += timeDiff * (getMaxShield() / (48.0 * 60 * 60));
    if(controlsSpecialist(SpecialistType::TINKERER)) fractionalShield -= timeDiff * (3.0 / (60 * 60));
    while(fractionalShield >= 1) {
        fractionalShield -= 1;
        
        shieldCharge++;

        if(shieldCharge > getMaxShield()) {
            fractionalShield -= int(fractionalShield);
            break;
        }
    }

    while(fractionalShield <= -1) {
        fractionalShield += 1;
        
        shieldCharge--;

        if(shieldCharge < 0) {
            fractionalShield -= int(fractionalShield);
            break;
        }
    }

    shieldCharge = std::fmax(0, std::fmin(shieldCharge, getMaxShield()));
    if(controlsSpecialist(SpecialistType::INSPECTOR)) shieldCharge = getMaxShield();

    return shieldCharge;
}

void Outpost::update(double timeDiff) {
    shieldCharge = getShieldAt(this->fractionalShield, timeDiff);
}

// returns shield charges removed, cannot remove more than the outpost currently has
int Outpost::removeShield(int amount) {
    if(amount > shieldCharge) amount = shieldCharge;

    shieldCharge -= amount;

    return amount;
}

int Outpost::getMaxShield() const {
    int shield = maxShieldCharge;
    
    if(!hasOwner()) return shield;
    
    shield += getOwner()->globalMaxShield();

    if(controlsSpecialist(SpecialistType::QUEEN)) shield += 20;

    shield += 10 * specialistCount(SpecialistType::SECURITY_CHIEF);

    shield += 40 * specialistCount(SpecialistType::KING);

    return std::fmax(0, shield);
}

int Outpost::getFireRange() const {
    return getSonarRange()/2;
}

int Outpost::getSonarRange() const {
    int range = getSettings()->defaultSonar;

    if(!hasOwner()) return range;

    range = int((getOwner()->globalSonar() + 0.5 * controlsSpecialist(SpecialistType::PRINCESS)) * range);

    return range;
}

int Outpost::getProductionAmount() {
    if(!hasOwner()) return 0;

    int productionAmount = getOwner()->globalProductionAmount();
    productionAmount += 6 * specialistCount(SpecialistType::FOREMAN);
    productionAmount += 3 * specialistCount(SpecialistType::TYCOON);

    return productionAmount;
}