#ifndef _VESSEL_OUTPOST_EVENT_H_
#define _VESSEL_OUTPOST_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../event.h"
#include "../player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"

class VesselOutpostEvent : public Event
{
private:
    Vessel* vessel;
    Outpost* outpost;

public:
    VesselOutpostEvent(){};
    VesselOutpostEvent(time_t timestamp, Vessel* vessel, Outpost* outpost) : Event(timestamp), vessel(vessel), outpost(outpost) {}

    bool referencesObject(int id) const override { return vessel->getID() == id || outpost->getID() == id; }

    void run(Game* game) const override {
        if(outpost->getOwnerID() == vessel->getOwnerID()) {
            outpost->addUnits(vessel->getUnits());
        } else {
            int val = std::min(vessel->getUnits(), outpost->getUnits() + outpost->getShield());
            std::cout << vessel->getUnits() << ", " << outpost->getUnits() << ", " << outpost->getShield() << std::endl;
            vessel->removeUnits(val);
            outpost->removeUnits(val - outpost->removeShield(val));

            int vesselUnits = vessel->getUnits();
            int outpostUnits = outpost ->getUnits();

            if(outpostUnits < vesselUnits) {
                outpost->setOwner(vessel->getOwner());
                outpost->addUnits(vesselUnits - outpostUnits);
            }
        }

        outpost->addSpecialists(vessel->getSpecialists());
        game->removeVessel(*vessel);
    }
};

#endif