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
        std::list<PositionalObject*> allObjects;
        std::list<bool> wasDisabled;
        
        for(const auto& pair : game->getOutposts()) allObjects.push_back(pair.second);
        for(const auto& pair : game->getVessels()) allObjects.push_back(pair.second);
        
        for(PositionalObject* obj : allObjects) {
            wasDisabled.push_back(obj->getGlobalDisabled());
            obj->setGlobalDisabled(false);
        }

        for(PositionalObject* source : allObjects) {
            if(source->controlsSpecialist(SpecialistType::CHANCELLOR)) {
                auto it = wasDisabled.begin();

                for(PositionalObject* target : allObjects) {
                    if(!target->getGlobalDisabled() && source->distance(target->getPosition()) <= game->getSettings()->defaultSonar) {
                        target->setGlobalDisabled(true);
                        if(!*it) target->setRefresh(true);
                    }

                    it++;
                }
            }
        }

        auto it = wasDisabled.begin();
        for(PositionalObject* obj : allObjects) {
            if(*it && !obj->getGlobalDisabled()) obj->setRefresh(true);
            it++;
        }

        // sentries will sequentially target vessels with highest units
        for(auto itA = game->getOutposts().begin(); itA != game->getOutposts().end(); itA++) {
            Outpost* outpost = itA->second;

            for(int i = 0; i < outpost->specialistCount(SpecialistType::SENTRY); i++) {
                Vessel* target = nullptr;
                double minDist = -1.0;

                for(auto& vessel : game->getVessels()) {
                    double dist = outpost->distance(vessel.second->getPosition());
                    if(vessel.second->getOwnerID() == outpost->getOwnerID() || dist > outpost->getFireRange()) continue;

                    if(target == nullptr || vessel.second->getUnits() > target->getUnits() || (vessel.second->getUnits() == target->getUnits() && dist < minDist)) {
                        target = vessel.second;
                        minDist = dist;
                    }
                }

                if(target != nullptr) {
                    target->removeUnits(int(ceil(0.05 * target->getUnits())));

                    if(target->getUnits() <= 0 && target->getSpecialists().empty()) game->removeVessel(target);
                }
            }
        }

        for(const auto& pair : game->getVessels()) {
            Vessel* vessel = pair.second;

            for(int i = 0; i < vessel->specialistCount(SpecialistType::MARAUDER); i++) {
                PositionalObject* target = nullptr;
                double minDist = -1.0;

                for(PositionalObject* o : allObjects) {
                    double dist = vessel->distance(o->getPosition());
                    if(o->getOwnerID() == vessel->getOwnerID() || dist > game->getSettings()->defaultSonar * game->getSettings()->fireRange) continue;

                    if(target == nullptr || o->getUnits() > target->getUnits() || (o->getUnits() == target->getUnits() && dist < minDist)) {
                        target = o;
                        minDist = dist;
                    }
                }

                if(target != nullptr) {
                    vessel->addUnits(target->removeUnits(int(ceil(0.04 * target->getUnits()))));


                    if(target->getUnits() <= 0 && target->getSpecialists().empty()) {
                        Vessel* v = dynamic_cast<Vessel*>(target);
                        if(v) game->removeVessel(v);
                    }
                }
            }
        }

        // schedule the same event to occur again
        game->addEvent(new OutpostRangeEvent(getTimestamp() + game->getSettings()->fireRate * game->getSettings()->baseFireRate / game->getSettings()->simulationSpeed));
    }
};

#endif