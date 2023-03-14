#ifndef _OUTPOST_RANGE_EVENT_H_
#define _OUTPOST_RANGE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"

class OutpostRangeEvent : public Event
{
private:
    std::shared_ptr<Outpost> outpost;

public:
    OutpostRangeEvent(){};
    OutpostRangeEvent(time_t timestamp, std::shared_ptr<Outpost> outpost) : Event(timestamp), outpost(outpost) {}

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        outpost = game->getOutpost(outpost->getID());
    }

    bool referencesObject(int id) const override { return outpost->getID() == id; }

    void run(Game* game) const override {
        // sentries will sequentially target vessels with highest units
        for(int i = 0; i < outpost->specialistCount(SpecialistType::SENTRY); i++) {
            std::shared_ptr<Vessel> target = nullptr;

            for(auto& vessel : game->getVessels()) {
                if(outpost->distance(game->getDimensions(), vessel.second->getPosition()) > outpost->getFireRange()) continue;
                if(target == nullptr || vessel.second->getUnits() > target->getUnits()) target = vessel.second;
            }

            if(target != nullptr) {
                target->removeUnits(int(ceil(0.05 * target->getUnits())));

                if(target->getUnits() <= 0) game->removeVessel(target);
            }
        }

        // schedule the same event to occur in 2 hours
        if(outpost->controlsSpecialist(SpecialistType::SENTRY)) {
            game->addEvent(new OutpostRangeEvent(getTimestamp() + 2 * 60 * 60, outpost));
        }
    }
};

#endif