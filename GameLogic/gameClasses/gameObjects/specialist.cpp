#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "player.h"

// overridden so that players can lose when specialists change hands
void Specialist::setOwner(Player* player) {
    if(type == SpecialistType::QUEEN && hasOwner()) {
        // find the nearest princess to turn into a queen, lose otherwise
        for(const std::shared_ptr<Outpost>& outpost : getOwner()->sortedOutposts(getContainer())) {
            if(outpost->controlsSpecialist(SpecialistType::PRINCESS)) {
                outpost->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                break;
            }
        }
    }

    Possessable::setOwner(player);
}