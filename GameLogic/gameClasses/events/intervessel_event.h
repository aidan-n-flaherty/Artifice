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
        BattleEvent(timestamp, vesselA, vesselB), vesselA(vesselA), vesselB(vesselB) {}

    Event* copy() override { return new IntervesselEvent(*this); }

    void updatePointers(Game *game) override {
        BattleEvent::updatePointers(game);
        vesselA = game->getVessel(vesselA->getID());
        vesselB = game->getVessel(vesselB->getID());
    }

    void run(Game* game) override {
        BattleEvent::run(game);

        if(game->ignoreVessel(vesselA->getID(), getTimestamp()) || game->ignoreVessel(vesselB->getID(), getTimestamp())) {
            setDisabled(true);
            return;
        } else if(vesselA->isGift() && vesselA->getTargetID() == vesselB->getID()) {
            setFriendly();

            vesselA->setTarget(vesselB->getReturnOutpost());
            vesselA->setOrigin(nullptr);
        } else if(vesselB->isGift() && vesselB->getTargetID() == vesselA->getID()) {
            setFriendly();

            vesselB->setTarget(vesselB->getReturnOutpost());
            vesselB->setOrigin(nullptr);
        } else if(vesselA->isGift() && vesselB->getTargetID() != vesselA->getID()) {
            setFriendly();
            return;
        } else if(vesselB->isGift() && vesselA->getTargetID() != vesselB->getID()) {
            setFriendly();
            return;
        } else if(vesselA->getOwnerID() == vesselB->getOwnerID()) {
            setFriendly();
            return;
        } else {
            // start with specialist phase
            specialistPhase(game);
            if(shouldEndCombat()) return;

            postSpecialistPhase(game);

            // remove units until one vessel has nothing left
            setPreVictoryUnits(vesselA->getOwnerID(), vesselA->getUnits());
            setPreVictoryUnits(vesselB->getOwnerID(), vesselB->getUnits());
            int val = std::min(vesselA->getUnits(), vesselB->getUnits());

            vesselA->removeUnits(val);
            vesselB->removeUnits(val);

            bool vesselAWins = (vesselB->getUnits() < vesselA->getUnits()) || (vesselA->getUnits() == vesselB->getUnits() && vesselB->getSpecialists().size() < vesselA->getSpecialists().size());
            bool tie = vesselA->getUnits() == vesselB->getUnits() && vesselB->getSpecialists().size() == vesselA->getSpecialists().size();

            if(tie) {
                defeatSpecialistPhase(game);
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

                if(!winner->isDeleted() && !loser->isDeleted()) {
                    if(!loser->getSpecialists().empty()) {
                        winner->getOwner()->addVessel(loser);
                        loser->returnHome();
                        if(winner->getOrigin() && (loser->getTargetID() == winner->getOriginID() || loser->getTargetID() == winner->getTargetID())) loser->setOrigin(winner->getOrigin());
                    } else game->removeVessel(loser);
                }

                // winner was a pirate
                if(!winner->isDeleted()) {
                    if(winner->getTargetID() == loser->getID()) {
                        winner->returnHome();
                        winner->setSpeedModifier(4);
                    }
                }

                for(const auto& pair : game->getPlayers()) {
                    if(!pair.second->controlsSpecialist(SpecialistType::QUEEN)) {
                        pair.second->setDefeated(game);
                    }
                }
            }
        }
    }
};

#endif