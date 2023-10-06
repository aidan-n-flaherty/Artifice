#include "vessel.h"
#include <list>
#include <cmath>
#include <algorithm>
#include "player.h"
#include "../game_settings.h"

int Outpost::getUnitsAt(double timeDiff) const {
    double fractionalProduction = this->fractionalProduction;
    
    return getUnitsAt(fractionalProduction, timeDiff);
}

int Outpost::getShieldAt(double timeDiff) const {
    double fractionalShield = this->fractionalShield;

    return getShieldAt(fractionalShield, timeDiff);
}

double Outpost::nextProductionEvent() const {
    return (1.0 - fractionalProduction) / (getOwner()->globalProductionSpeed() * getSettings()->simulationSpeed / (8.0 * 60 * 60));
}

/* The following two functions do not modify this instance unless you pass in
** references to the actual member variables, allowing them to be used for interpolation
** or updating the game state.
*/
int Outpost::getUnitsAt(double& fractionalProduction, double timeDiff) const {
    timeDiff *= getSettings()->simulationSpeed;

    int units = getUnits();

    if(getOwnerID() == -1 || type != OutpostType::FACTORY) return units;

    // while loops necessary in case a tick doesn't happen for several hours
    fractionalProduction += timeDiff * getOwner()->globalProductionSpeed() * (1.0 / (8.0 * 60 * 60));
    while(fractionalProduction >= 1) {
        fractionalProduction -= 1;

        int productionAmount = getOwner()->globalProductionAmount();
        productionAmount += 6 * specialistCount(SpecialistType::FOREMAN);
        productionAmount += 3 * specialistCount(SpecialistType::TYCOON);

        units += std::fmin(std::fmax(0, getOwner()->getCapacity() - getOwner()->getUnits()), productionAmount);
    }

    return units;
}

int Outpost::getShieldAt(double& fractionalShield, double timeDiff) const {
    timeDiff *= getSettings()->simulationSpeed;

    double shieldCharge = this->shieldCharge;

    if(getOwnerID() == -1) return shieldCharge;

    fractionalShield += timeDiff * (getMaxShield() / (48.0 * 60 * 60));
    if(controlsSpecialist(SpecialistType::TINKERER)) fractionalShield -= timeDiff * (3.0 / (60 * 60));
    while(fractionalShield >= 1) {
        fractionalShield -= 1;
        
        shieldCharge++;
    }

    while(fractionalShield <= -1) {
        fractionalShield += 1;
        
        shieldCharge--;
    }

    shieldCharge = std::fmax(0, std::fmin(shieldCharge, getMaxShield()));
    if(controlsSpecialist(SpecialistType::INSPECTOR)) shieldCharge = getMaxShield();

    return shieldCharge;
}

void Outpost::update(double timeDiff) {
    setUnits(getUnitsAt(this->fractionalProduction, timeDiff));

    shieldCharge = getShieldAt(this->fractionalShield, timeDiff);
}

// returns shield charges removed, cannot remove more than the outpost currently has
int Outpost::removeShield(int amount) {
    if(amount > shieldCharge) amount = shieldCharge;

    shieldCharge -= amount;

    return amount;
}

int Outpost::getMaxShield() const {
    int shield = maxShieldCharge + getOwner()->globalMaxShield();

    shield += 10 * specialistCount(SpecialistType::SECURITY_CHIEF);

    shield += 40 * specialistCount(SpecialistType::KING);

    return std::fmax(0, shield);
}

int Outpost::getFireRange() const {
    return getSonarRange()/2;
}

int Outpost::getSonarRange() const {
    int range = getSettings()->defaultSonar;

    range = int((getOwner()->globalSonar() + 0.5 * controlsSpecialist(SpecialistType::PRINCESS)) * range);

    return range;
}