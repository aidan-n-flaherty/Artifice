#include "specialist.h"
#include "outpost.h"
#include "../game.h"
#include "../player.h"

// overridden so that players can lose when specialists change hands
void Specialist::setOwner(std::shared_ptr<Player> player) {
    if(type == SpecialistType::QUEEN) {
        std::list<std::shared_ptr<Outpost>> outposts;

        for(const std::shared_ptr<Outpost>& o : getOwner()->getOutposts()) {
            outposts.push_back(o);
        }

        if(container != nullptr) {
            outposts.sort([=](const std::shared_ptr<Outpost>& a, const std::shared_ptr<Outpost>& b) -> bool
            { 
                return container->distance(Game::getDimensions(), a->getPosition()) < container->distance(Game::getDimensions(), b->getPosition()); 
            });
        }

        // find the nearest princess to turn into a queen, lose otherwise
        bool promotedPrincess = false;
        for(const std::shared_ptr<Outpost>& outpost : outposts) {
            if(outpost->controlsSpecialist(SpecialistType::PRINCESS)) {
                outpost->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                promotedPrincess = true;
                break;
            }
        }

        if(!promotedPrincess) getOwner()->setDefeated();
    }

    Possessable::setOwner(player);
}