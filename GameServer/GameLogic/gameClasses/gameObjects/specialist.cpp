#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "player.h"

void Specialist::updatePointers(Game *game) {
    Possessable::updatePointers(game);
    if(container != nullptr) container = game->getPosObject(container->getID());
}

// overridden so that players can lose when specialists change hands
void Specialist::setOwner(Player* player) {
    if(type == SpecialistType::QUEEN && hasOwner()) {
        // find the nearest princess to turn into a queen, lose otherwise
        for(Outpost* outpost : getOwner()->sortedOutposts(getContainer())) {
            if(outpost->controlsSpecialist(SpecialistType::PRINCESS)) {
                outpost->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                break;
            }
        }
    }

    Possessable::setOwner(player);
}