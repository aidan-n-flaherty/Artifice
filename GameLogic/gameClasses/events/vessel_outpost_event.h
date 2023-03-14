#ifndef _VESSEL_OUTPOST_EVENT_H_
#define _VESSEL_OUTPOST_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../event.h"
#include "../gameObjects/player.h"
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

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    bool referencesObject(int id) const override { return vessel->getID() == id; }

    void run(Game* game) const override {
        std::cout << "Vessel-Outpost combat: " << vessel->getOwner()->getName() << ", " << outpost->getOwner()->getName() << std::endl;
        if(outpost->getOwnerID() == vessel->getOwnerID() || vessel->isGift()) {
            outpost->addUnits(vessel->getUnits());
            if(vessel->isGift()) outpost->getOwner()->addSpecialists(vessel->getSpecialists());
        } else {
            int originalVesselUnits = vessel->getUnits(), originalOutpostUnits = outpost->getUnits();
            int vesselUnits = vessel->getUnits(), outpostUnits = outpost->getUnits();

            vessel->specialistPhase(vesselUnits, outpostUnits, outpost);
            outpost->specialistPhase(outpostUnits, vesselUnits, vessel);
            vesselUnits = vessel->setUnits(vesselUnits);
            outpostUnits = outpost->setUnits(outpostUnits);

            vessel->postSpecialistPhase(vesselUnits, outpostUnits, outpost);
            outpost->postSpecialistPhase(outpostUnits, vesselUnits, vessel);
            vesselUnits = vessel->setUnits(vesselUnits);
            outpostUnits = outpost->setUnits(outpostUnits);

            int val = std::min(vesselUnits, outpostUnits + outpost->getShield());
            vesselUnits -= vessel->removeUnits(val);
            outpostUnits -= outpost->removeUnits(val - outpost->removeShield(val));

            bool vesselWins = (outpostUnits < vesselUnits) || (outpostUnits == vesselUnits && outpost->getSpecialists().size() < vessel->getSpecialists().size());

            std::shared_ptr<PositionalObject> loser = vesselWins ? (std::shared_ptr<PositionalObject>)outpost : (std::shared_ptr<PositionalObject>)vessel;
            std::shared_ptr<PositionalObject> winner = !vesselWins ? (std::shared_ptr<PositionalObject>)outpost : (std::shared_ptr<PositionalObject>)vessel; 
            int winnerDelta = vesselWins ? originalVesselUnits - vesselUnits : originalOutpostUnits - outpostUnits;
            int loserDelta = !vesselWins ? originalVesselUnits - vesselUnits : originalOutpostUnits - outpostUnits;

            loser->defeatSpecialistPhase(loserDelta, winnerDelta, winner);
            winner->victorySpecialistPhase(winnerDelta, loserDelta, loser);

            vessel->postCombatSpecialistPhase(game, outpost);
            outpost->postCombatSpecialistPhase(game, vessel);

            if(vessel->hasOwner() && !vessel->getOwner()->controlsSpecialist(SpecialistType::QUEEN)) {
                vessel->getOwner()->setDefeated(game);
            }

            if(outpost->hasOwner() && !outpost->getOwner()->controlsSpecialist(SpecialistType::QUEEN)) {
                outpost->getOwner()->setDefeated(game);
            }

            if(vesselWins) {
                outpost->setOwner(vessel->getOwner());
                outpost->addUnits(vessel->getUnits() - outpost->getUnits());
            }
        }

        outpost->addSpecialists(vessel->removeSpecialists());
        game->removeVessel(vessel);
    }
};

#endif