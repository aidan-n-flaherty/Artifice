#ifndef _INTERVESSEL_COMBAT_EVENT_H_
#define _INTERVESSEL_COMBAT_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../event.h"
#include "../gameObjects/vessel.h"
#include "../event.h"
#include "../game.h"

class IntervesselEvent : public Event
{
private:
    std::shared_ptr<Vessel> vesselA;
    std::shared_ptr<Vessel> vesselB;

public:
    IntervesselEvent(){};
    IntervesselEvent(time_t timestamp, std::shared_ptr<Vessel> vesselA, std::shared_ptr<Vessel> vesselB) :
        Event(timestamp), vesselA(vesselA), vesselB(vesselB) {}
    IntervesselEvent(const std::shared_ptr<IntervesselEvent> other, Game* game);
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vesselA = game->getVessel(vesselA->getID());
        vesselB = game->getVessel(vesselB->getID());
    }

    bool referencesObject(int id) const override { return vesselA->getID() == id || vesselB->getID() == id; }

    void run(Game* game) const override {
        if(vesselA->getOwnerID() == vesselB->getOwnerID()) {
            vesselA->addUnits(vesselB->removeUnits(vesselB->getUnits()));
            vesselA->addSpecialists(vesselB->getSpecialists());

            game->removeVessel(vesselB);
        } else {
            int val = std::min(vesselA->getUnits(), vesselB->getUnits());

            vesselA->removeUnits(val);
            vesselB->removeUnits(val);
            int unitsA = vesselA->getOwner()->attackPower(vesselA->getUnits());
            int unitsB = vesselB->getOwner()->attackPower(vesselB->getUnits());

            std::cout << unitsA << ", " << unitsB << " at ";
            std::cout << "(" << vesselA->getPosition().getX() << ", " << vesselA->getPosition().getY() << ")" << std::endl;

            // use specialists as tie breaker
            if(unitsA == unitsB) {
                unitsA += vesselA->getSpecialists().size();
                unitsB += vesselB->getSpecialists().size();
            }

            if(unitsA == unitsB) {
                // in an event of a tie, both subs are sent back
                vesselA->setTarget(vesselA->getOrigin());
                vesselB->setTarget(vesselB->getOrigin());
            }
            else if(unitsA < unitsB) game->removeVessel(vesselA);
            else game->removeVessel(vesselB);
        }
    }
};

#endif