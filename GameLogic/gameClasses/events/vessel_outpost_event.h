#ifndef _VESSEL_OUTPOST_EVENT_H_
#define _VESSEL_OUTPOST_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/outpost.h"
#include "../game.h"
#include "battle_event.h"
#include "release_event.h"

class VesselOutpostEvent : public BattleEvent
{
private:
    Vessel* vessel;
    Outpost* outpost;

    int vID;

public:
    VesselOutpostEvent(){};
    VesselOutpostEvent(double timestamp, Vessel* vessel, Outpost* outpost) : BattleEvent(timestamp, vessel, outpost), vessel(vessel), outpost(outpost), vID(vessel->getID()) {
        if(vessel->isGift()) setFriendly();
    }

    Event* copy() override { return new VesselOutpostEvent(*this); }

    void updatePointers(Game *game) override {
        BattleEvent::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
        outpost = game->getOutpost(outpost->getID());
    }

    //bool referencesObject(int id) const override { return vID == id; }

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

            setPreVictoryUnits(vessel->getOwnerID(), vessel->getUnits());
            setPreVictoryUnits(outpost->getOwnerID(), outpost->getUnits());
            setShields(outpost->getOwnerID(), outpost->getShield());

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

            if(!vessel->isDeleted() && !outpost->isDeleted()) {
                if(vesselWins) {
                    vesselOwner->addOutpost(outpost);
                    outpost->addUnits(vessel->getUnits() - outpost->getUnits());
                }
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
                outpost->getOwner()->addSpecialist(s);
            }
        }
        if (outpost->controlsSpecialist(SpecialistType::DIPLOMAT)) {
            //Releases all captive specialists you own that are being held at an outpost within Diplomat's outpost's sonar range.
            //Check all outposts
            for (auto pair : game->getOutposts()) {
                //if pair in outpost sonar range
                Outpost* target = pair.second;
                if ((outpost->distance(target->getPosition()) <= outpost->getSonarRange()) && (target->getOwnerID() != outpost->getOwnerID())) {
                    //release any "captured" specialists
                    for (Specialist* s : target->getSpecialists()) {
                        if (s->getOwnerID() == outpost->getOwnerID()) {
                           //send a release event at this timestamp
                            game->addEvent(new ReleaseEvent(nullptr, getTimestamp(), s, target));
                        }
                    }
                }
            }
        }
        game->removeVessel(vessel);
    }
};

#endif