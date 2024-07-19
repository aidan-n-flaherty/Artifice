#ifndef _ACTIVATE_EVENT_H_
#define _ACTIVATE_EVENT_H_

#include <cstdlib>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"

class ActivateEvent : public Event
{
private:
    Specialist* specialist;

public:
    ActivateEvent(){};
    ActivateEvent(Order* o, double timestamp, Specialist* specialist) : Event(o, timestamp), specialist(specialist) {}
    
    Event* copy() override { return new ActivateEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        if(specialist->getType() == SpecialistType::DETONATOR) {
            PositionalObject* obj = specialist->getContainer();

            if(!obj) return;

            int range = game->getSettings()->defaultSonar/5;

            for(const auto &v : game->getVessels()) {
                if(obj->distance(v.second->getPosition()) <= range) {
                    game->removeVessel(v.second);
                }
            }

            for(const auto &o : game->getOutposts()) {
                if(obj->distance(o.second->getPosition()) <= range) {
                    if(o.second->getType() == OutpostType::MINE && o.second->hasOwner()) {
                        o.second->getOwner()->removeResources(o.second->getOwner()->getResources() * game->getSettings()->resourceReductionAmount);
                    }
                    
                    o.second->setType(OutpostType::BROKEN);
                    o.second->setUnits(0);

                    while(!o.second->getSpecialists().empty()) game->removeSpecialist(o.second->getSpecialists().front());
                }
            }

            for(const auto& pair : game->getPlayers()) {
                if(!pair.second->controlsSpecialist(SpecialistType::QUEEN)) {
                    pair.second->setDefeated(game);
                }
            }
        }
    }
};

#endif