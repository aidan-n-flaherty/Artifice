#include "event.h"
#include "game.h"
#include "gameObjects/specialist.h"
#include <list>

int Event::counter = 0;

void Event::lossEffect(Game* game, std::shared_ptr<PositionalObject> o) const {
    if(o->hasSpecialist(SpecialistType::QUEEN)) {
        std::list<std::shared_ptr<Outpost>> outposts;

        for(auto pair : game->getOutposts()) {
            outposts.push_back(pair.second);
        }

        outposts.sort([=](const std::shared_ptr<Outpost> & a, const std::shared_ptr<Outpost> & b) -> bool
        { 
            return o->distance(game->getDimensions(), a->getPosition()) < o->distance(game->getDimensions(), b->getPosition()); 
        });

        bool promotedPrincess = false;
        for(std::shared_ptr<Outpost> outpost : outposts) {
            if(outpost->hasSpecialist(SpecialistType::PRINCESS)) {
                outpost->getSpecialist(SpecialistType::PRINCESS)->setType(SpecialistType::QUEEN);
                promotedPrincess = true;
                break;
            }
        }

        if(!promotedPrincess) o->getOwner()->setDefeated();
    }
}