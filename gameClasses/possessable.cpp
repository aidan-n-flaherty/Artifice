#include "possessable.h"
#include "game.h"

void Possessable::updatePointers(Game* game) {
    owner = game->getPlayer(owner->getID());
}