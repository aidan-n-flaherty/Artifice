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
            // start with specialist phase
            int originalUnitsA = vesselA->getUnits(), originalUnitsB = vesselB->getUnits();
            int unitsA = vesselA->getUnits(), unitsB = vesselB->getUnits();

            vesselA->specialistPhase(unitsA, unitsB, vesselB);
            vesselB->specialistPhase(unitsB, unitsA, vesselA);
            unitsA = vesselA->setUnits(unitsA);
            unitsB = vesselB->setUnits(unitsB);

            vesselA->postSpecialistPhase(unitsA, unitsB, vesselB);
            vesselB->postSpecialistPhase(unitsB, unitsA, vesselA);
            unitsA = vesselA->setUnits(unitsA);
            unitsB = vesselB->setUnits(unitsB);

            // remove units until one vessel has nothing left
            int val = std::min(unitsA, unitsB);

            unitsA -= vesselA->removeUnits(val);
            unitsB -= vesselB->removeUnits(val);

            std::cout << unitsA << ", " << unitsB << " at ";
            std::cout << "(" << vesselA->getPosition().getX() << ", " << vesselA->getPosition().getY() << ")" << std::endl;

            bool vesselAWins = (unitsB < unitsA) || (unitsB == unitsA && vesselB->getSpecialists().size() < vesselA->getSpecialists().size());
            bool tie = unitsA == unitsB && vesselB->getSpecialists().size() == vesselA->getSpecialists().size();

            if(tie) {
                // in an event of a tie, both subs are sent back
                vesselA->setTarget(vesselA->getOrigin());
                vesselB->setTarget(vesselB->getOrigin());
            } else {
                std::shared_ptr<Vessel> winner = vesselAWins ? vesselA : vesselB;
                std::shared_ptr<Vessel> loser = vesselAWins ? vesselB : vesselA; 
                int winnerDelta = vesselAWins ? originalUnitsA - unitsA : originalUnitsB - unitsB;
                int loserDelta = !vesselAWins ? originalUnitsA - unitsA : originalUnitsB - unitsB;

                loser->defeatSpecialistPhase(loserDelta, winnerDelta, winner);
                winner->victorySpecialistPhase(winnerDelta, loserDelta, loser);

                if(!loser->getSpecialists().empty()) loser->setOwner(winner->getOwner());
                else game->removeVessel(loser);
            }

            vesselA->postCombatSpecialistPhase(game);
            vesselB->postCombatSpecialistPhase(game);
        }
    }
};

#endif