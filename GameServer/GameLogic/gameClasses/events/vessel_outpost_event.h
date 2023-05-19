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
    Vessel* vessel;
    Outpost* outpost;

public:
    VesselOutpostEvent(){};
    VesselOutpostEvent(time_t timestamp, Vessel* vessel, Outpost* outpost) : Event(timestamp), vessel(vessel), outpost(outpost) {}

    Event* copy() override { return new VesselOutpostEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    bool referencesObject(int id) const override { return vessel->getID() == id; }

    void run(Game* game) const override {
        std::cout << "Vessel-Outpost combat: " << game->getTime() << ", " << (vessel->hasOwner() ? vessel->getOwner()->getName() : "none") << ", " << (outpost->hasOwner() ? outpost->getOwner()->getName() : "none") << std::endl;
        if(outpost->getOwnerID() == vessel->getOwnerID() || vessel->isGift()) {
            outpost->addUnits(vessel->getUnits());
            if(vessel->isGift()) outpost->getOwner()->addSpecialists(vessel->getSpecialists());

            outpost->addSpecialists(vessel->removeSpecialists());
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

            std::cout << vesselUnits << ", " << outpostUnits << " at ";
            std::cout << "(" << vessel->getPosition().getX() << ", " << outpost->getPosition().getY() << ")" << std::endl;

            bool vesselWins = (outpostUnits < vesselUnits) || (outpostUnits == vesselUnits && outpost->getSpecialists().size() < vessel->getSpecialists().size());

            PositionalObject* loser = vesselWins ? (PositionalObject*)outpost : (PositionalObject*)vessel;
            PositionalObject* winner = !vesselWins ? (PositionalObject*)outpost : (PositionalObject*)vessel; 
            int winnerDelta = vesselWins ? originalVesselUnits - vesselUnits : originalOutpostUnits - outpostUnits;
            int loserDelta = !vesselWins ? originalVesselUnits - vesselUnits : originalOutpostUnits - outpostUnits;

            loser->defeatSpecialistPhase(loserDelta, winnerDelta, winner);
            winner->victorySpecialistPhase(winnerDelta, loserDelta, loser);

            vessel->postCombatSpecialistPhase(game, outpost);
            outpost->postCombatSpecialistPhase(game, vessel);

            outpost->addSpecialists(vessel->removeSpecialists());

            Player* vesselOwner = vessel->getOwner();
            Player* outpostOwner = outpost->getOwner();

            if(vesselWins) {
                vesselOwner->addOutpost(outpost);
                outpost->addUnits(vessel->getUnits() - outpost->getUnits());
            }

            if(vesselOwner && !vesselOwner->controlsSpecialist(SpecialistType::QUEEN)) {
                vesselOwner->setDefeated(game);
            }

            if(outpostOwner && !outpostOwner->controlsSpecialist(SpecialistType::QUEEN)) {
                outpostOwner->setDefeated(game);
            }
        }

        game->removeVessel(vessel);
    }
};

#endif