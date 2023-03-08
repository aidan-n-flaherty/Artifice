#include "possessable.h"
#include "game.h"

void Possessable::updatePointers(Game* game) {
    if(game->hasPlayer(owner->getID())) owner = game->getPlayer(owner->getID());
}