#ifndef _INTERVESSEL_COMBAT_EVENT_H_
#define _INTERVESSEL_COMBAT_EVENT_H_

#include <cstdlib>
#include <cmath>
#include "../event.h"
#include "../gameObjects/vessel.h"
#include "../event.h"
#include "../game.h"

class IntervesselEvent : public Event
{
private:
    Vessel *vesselA;
    Vessel *vesselB;

public:
    IntervesselEvent(){};
    IntervesselEvent(time_t timestamp, Vessel* vesselA, Vessel* vesselB) :
        Event(timestamp), vesselA(vesselA), vesselB(vesselB) {}

    bool referencesObject(int id) const override { return vesselA->getID() == id || vesselB->getID() == id; }

    void run(Game* game) const override {
        if(vesselA->getOwnerID() == vesselB->getOwnerID()) {
            vesselA->addUnits(vesselB->removeUnits(vesselB->getUnits()));
            vesselA->addSpecialists(vesselB->getSpecialists());

            game->removeVessel(*vesselB);
        } else {
            int val = std::min(vesselA->getUnits(), vesselB->getUnits());

            int unitsA = vesselA->removeUnits(val);
            int unitsB = vesselB->removeUnits(val);

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
            else if(unitsA < unitsB) game->removeVessel(*vesselA);
            else game->removeVessel(*vesselB);
        }
    }
};

#endif