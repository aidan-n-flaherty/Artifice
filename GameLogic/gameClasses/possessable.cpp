#include "possessable.h"
#include "game.h"

void Possessable::updatePointers(Game* game) {
    if(hasOwner() && game->hasPlayer(owner->getID())) owner = game->getPlayer(owner->getID()).get();
}

int Possessable::getOwnerID() const {
    return hasOwner() ? owner->getID() : -1;
}