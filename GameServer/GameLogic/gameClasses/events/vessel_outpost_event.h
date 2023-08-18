#ifndef _VESSEL_OUTPOST_EVENT_H_
#define _VESSEL_OUTPOST_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"
#include "battle_event.h"

class VesselOutpostEvent : public BattleEvent
{
private:
    Vessel* vessel;
    Outpost* outpost;

public:
    VesselOutpostEvent(){};
    VesselOutpostEvent(double timestamp, Vessel* vessel, Outpost* outpost) : BattleEvent(timestamp, vessel, outpost), vessel(vessel), outpost(outpost) {}

    Event* copy() override { return new VesselOutpostEvent(*this); }

    void updatePointers(Game *game) override {
        BattleEvent::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    bool referencesObject(int id) const override { return vessel->getID() == id; }

    void run(Game* game) override {
        BattleEvent::run(game);

        std::cout << "Vessel-Outpost combat: " << game->getTime() << ", " << (vessel->hasOwner() ? vessel->getOwner()->getName() : "none") << ", " << (outpost->hasOwner() ? outpost->getOwner()->getName() : "none") << std::endl;
        std::cout << outpost->getPosition().getX() << ", " << outpost->getPosition().getY() << std::endl;

        if(outpost->getOwnerID() == vessel->getOwnerID() || vessel->isGift()) {
            outpost->addUnits(vessel->getUnits());
            if(vessel->isGift()) outpost->getOwner()->addSpecialists(vessel->getSpecialists());

            outpost->addSpecialists(vessel->removeSpecialists());
        } else {
            // start with specialist phase
            specialistPhase(game);
            postSpecialistPhase(game);

            int val = std::min(vessel->getUnits(), outpost->getUnits() + outpost->getShield());
            vessel->removeUnits(val);
            outpost->removeUnits(val - outpost->removeShield(val));

            bool vesselWins = (outpost->getUnits() < vessel->getUnits()) || (outpost->getUnits() == vessel->getUnits() && outpost->getSpecialists().size() < vessel->getSpecialists().size());

            setVictor(vesselWins ? vessel->getOwnerID() : outpost->getOwnerID());

            defeatSpecialistPhase(game);
            victorySpecialistPhase(game);
            postCombatSpecialistPhase(game);

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

        if(outpost->controlsSpecialist(SpecialistType::HYPNOTIST)) {
            for(Specialist* s : outpost->getSpecialists()) {
                s->setOwner(outpost->getOwner());
            }
        }

        game->removeVessel(vessel);
    }
};

#endif