#include "vessel.h"
#include <list>

void Outpost::update(double timeDiff) {
    // while loops necessary in case a tick doesn't happen for several hours
    fractionalProduction += timeDiff * (6.0 / (8.0 * 60 * 60));
    while(fractionalProduction >= 6) {
        fractionalProduction -= 6;
        addUnits(6);
    }

    fractionalShield += timeDiff * (1.0 / (60 * 60));
    while(fractionalShield >= 1) {
        fractionalShield -= 1;
        if(shieldCharge < maxShieldCharge) {
            shieldCharge++;
        }
    }
}

// returns shield charges removed, cannot remove more than the outpost currently has
int Outpost::removeShield(int amount) {
    if(amount > shieldCharge) amount = shieldCharge;

    shieldCharge -= amount;

    return amount;
}