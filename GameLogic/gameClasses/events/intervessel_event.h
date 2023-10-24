#ifndef _INTERVESSEL_COMBAT_EVENT_H_
#define _INTERVESSEL_COMBAT_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "../event.h"
#include "../gameObjects/vessel.h"
#include "../event.h"
#include "../game.h"
#include "battle_event.h"

class IntervesselEvent : public BattleEvent
{
private:
    Vessel* vesselA;
    Vessel* vesselB;

public:
    IntervesselEvent(){};
    IntervesselEvent(double timestamp, Vessel* vesselA, Vessel* vesselB) :
        BattleEvent(timestamp, vesselA, vesselB), vesselA(vesselA), vesselB(vesselB) {
        if(vesselA->isGift() || vesselB->isGift()) setFriendly();
    }

    Event* copy() override { return new IntervesselEvent(*this); }

    void updatePointers(Game *game) override {
        BattleEvent::updatePointers(game);
        vesselA = game->getVessel(vesselA->getID());
        vesselB = game->getVessel(vesselB->getID());
    }

    void run(Game* game) override {
        BattleEvent::run(game);

        std::cout << "Vessel-Vessel combat: " << game->getTime() << ", " << (vesselA->hasOwner() ? vesselA->getOwner()->getName() : "none") << ", " << (vesselB->hasOwner() ? vesselB->getOwner()->getName() : "none") << std::endl;

        if(vesselA->isGift() && vesselB->isGift()) {
            return;
        } else if(vesselA->isGift() && !vesselB->isGift()) {
            vesselB->getOwner()->addVessel(vesselA);
        } else if(vesselB->isGift() && !vesselA->isGift()) {
            vesselA->getOwner()->addVessel(vesselB);
        } else if(vesselA->getOwnerID() == vesselB->getOwnerID()) {
            vesselA->addUnits(vesselB->removeUnits(vesselB->getUnits()));
            vesselA->addSpecialists(vesselB->getSpecialists());

            game->removeVessel(vesselB);
        } else {
            // start with specialist phase
            specialistPhase(game);
            postSpecialistPhase(game);

            // remove units until one vessel has nothing left
            int val = std::min(vesselA->getUnits(), vesselB->getUnits());

            vesselA->removeUnits(val);
            vesselB->removeUnits(val);

            bool vesselAWins = (vesselB->getUnits() < vesselA->getUnits()) || (vesselA->getUnits() == vesselB->getUnits() && vesselB->getSpecialists().size() < vesselA->getSpecialists().size());
            bool tie = vesselA->getUnits() == vesselB->getUnits() && vesselB->getSpecialists().size() == vesselA->getSpecialists().size();

            if(tie) {
                postCombatSpecialistPhase(game);

                // in an event of a tie, both subs are sent back
                if(!vesselA->getSpecialists().empty()) vesselA->returnHome();
                else game->removeVessel(vesselA);

                if(!vesselB->getSpecialists().empty()) vesselB->returnHome();
                else game->removeVessel(vesselB);
            } else {
                Vessel* winner = vesselAWins ? vesselA : vesselB;
                Vessel* loser = vesselAWins ? vesselB : vesselA; 
                setVictor(winner->getOwnerID());

                defeatSpecialistPhase(game);
                victorySpecialistPhase(game);
                postCombatSpecialistPhase(game);

                Player* vesselAOwner = vesselA->getOwner();
                Player* vesselBOwner = vesselB->getOwner();

                if(!winner->isDeleted() && !loser->isDeleted()) {
                    if(!loser->getSpecialists().empty()) {
                        winner->getOwner()->addVessel(loser);
                        loser->returnHome();
                    } else game->removeVessel(loser);
                }

                // winner was a pirate
                if(!winner->isDeleted()) {
                    if(winner->getTargetID() == loser->getID()) {
                        winner->returnHome();
                        winner->setSpeedModifier(4);
                    }
                }

                if(vesselAOwner && !vesselAOwner->controlsSpecialist(SpecialistType::QUEEN)) {
                    vesselAOwner->setDefeated(game);
                }

                if(vesselBOwner && !vesselBOwner->controlsSpecialist(SpecialistType::QUEEN)) {
                    vesselBOwner->setDefeated(game);
                }
            }
        }
    }
};

#endif