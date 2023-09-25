#ifndef _OUTPOST_RANGE_EVENT_H_
#define _OUTPOST_RANGE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../game_settings.h"
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"

class OutpostRangeEvent : public Event
{
public:
    OutpostRangeEvent(){};
    OutpostRangeEvent(double timestamp) : Event(nullptr, timestamp) {}

    Event* copy() override { return new OutpostRangeEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
    }

    void run(Game* game) override {
        // sentries will sequentially target vessels with highest units
        for(auto itA = game->getOutposts().begin(); itA != game->getOutposts().end(); itA++) {
            Outpost* outpost = itA->second;

            for(int i = 0; i < outpost->specialistCount(SpecialistType::SENTRY); i++) {
                Vessel* target = nullptr;

                for(auto& vessel : game->getVessels()) {
                    if(outpost->distance(vessel.second->getPosition()) > outpost->getFireRange()) continue;
                    if(target == nullptr || vessel.second->getUnits() > target->getUnits()) target = vessel.second;
                }

                if(target != nullptr) {
                    target->removeUnits(int(ceil(0.05 * target->getUnits())));

                    if(target->getUnits() <= 0) game->removeVessel(target);
                }
            }
        }

        // schedule the same event to occur in 2 hours
        game->addEvent(new OutpostRangeEvent(getTimestamp() + GameSettings::fireRate / GameSettings::simulationSpeed));
    }
};

#endif