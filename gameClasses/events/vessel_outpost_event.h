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
    std::shared_ptr<Vessel> vessel;
    std::shared_ptr<Outpost> outpost;

public:
    VesselOutpostEvent(){};
    VesselOutpostEvent(time_t timestamp, std::shared_ptr<Vessel> vessel, std::shared_ptr<Outpost> outpost) : Event(timestamp), vessel(vessel), outpost(outpost) {}
    VesselOutpostEvent(const std::shared_ptr<VesselOutpostEvent> other, Game* game);
    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    bool referencesObject(int id) const override { return vessel->getID() == id || outpost->getID() == id; }

    void run(Game* game) const override {
        std::cout << "Vessel-Outpost combat: " << vessel->getOwnerID() << ", " << outpost->getOwnerID() << std::endl;
        if(outpost->getOwnerID() == vessel->getOwnerID()) {
            outpost->addUnits(vessel->getUnits());
        } else {
            int val = std::min(vessel->getUnits(), outpost->getUnits() + outpost->getShield());
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
        game->removeVessel(vessel);
    }
};

#endif