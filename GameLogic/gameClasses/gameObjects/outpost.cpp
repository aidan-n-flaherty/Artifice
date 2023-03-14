#include "vessel.h"
#include <list>
#include <cmath>
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

/* The following two functions do not modify this instance unless you pass in
** references to the actual member variables, allowing them to be used for interpolation
** or updating the game state.
*/
int Outpost::getUnitsAt(double& fractionalProduction, double timeDiff) const {
    int units = getUnits();

    // while loops necessary in case a tick doesn't happen for several hours
    fractionalProduction += timeDiff * getOwner()->globalProductionSpeed() * (6.0 / (8.0 * 60 * 60));
    while(fractionalProduction >= 6) {
        fractionalProduction -= 6;

        int productionAmount = getOwner()->globalProductionAmount();
        if(controlsSpecialist(SpecialistType::FOREMAN)) productionAmount += 6;
        if(controlsSpecialist(SpecialistType::TYCOON)) productionAmount += 3;

        units += std::fmin(std::fmax(0, getOwner()->getCapacity() - getOwner()->getUnits()), productionAmount);
    }

    return units;
}

int Outpost::getShieldAt(double& fractionalShield, double timeDiff) const {
    double shieldCharge = this->shieldCharge;

    fractionalShield += timeDiff * (1.0 / (60 * 60));
    while(fractionalShield >= 1) {
        fractionalShield -= 1;
        
        if(controlsSpecialist(SpecialistType::TINKERER)) shieldCharge -= 3;
        else shieldCharge++;
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
    return GameSettings::defaultSonar/2;
}

int Outpost::getSonarRange() const {
    int range = GameSettings::defaultSonar;

    range = int((getOwner()->globalSonar() + 0.5 * controlsSpecialist(SpecialistType::PRINCESS)) * range);

    return range;
}

void Outpost::specialistPhase(int& units, int& otherUnits, std::shared_ptr<Vessel> other) {
    if(controlsSpecialist(SpecialistType::REVERED_ELDER) || other->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    PositionalObject::specialistPhase(units, otherUnits, other);
}