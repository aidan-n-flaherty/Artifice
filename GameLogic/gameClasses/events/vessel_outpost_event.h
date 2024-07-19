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

    //checks to see if the players whose ids are provided are on the same team
    bool same_team(Outpost* outpost_to_check, Vessel* vessel_to_check){

        //checks to make sure that both players exist.
        //(getOwnerID() returns -1 if players[ID] points to a null ptr)

        int outpostID = outpost_to_check->getOwnerID();
        int vessel_playerID = vessel_to_check->getOwnerID();

        if((outpostID == -1) || (vessel_playerID == -1)){
            return false;
        }
        //both players exist
        else{
            //if both players belong to the same team, return true
            return outpost_to_check->getOwner()->getTeamID() == vessel_to_check->getOwner()->getTeamID();
        }
    }

    //bool referencesObject(int id) const override { return vID == id; }

    void run(Game* game) override {
        BattleEvent::run(game);

        if(game->ignoreVessel(vID, getTimestamp())) {
            setDisabled(true);
            vessel->setDisabled(true);
            return;
        } else if(outpost->getOwnerID() == vessel->getOwnerID() || vessel->isGift() || (((game->getSettings())->number_of_teams > 1) && same_team(outpost, vessel))) {
            if(outpost->hasOwner() && outpost->getOwnerID() != vessel->getOwnerID()) {
                for(Specialist* s : vessel->getSpecialists()) {
                    if(s->hasOwner() && s->getOwnerID() == vessel->getOwnerID()) {
                        outpost->getOwner()->addSpecialist(s);
                    }
                }

                outpost->addUnits(std::max(0, std::min(vessel->getUnits(), game->getSettings()->giftPadding + outpost->getOwner()->getCapacity() - outpost->getOwner()->getUnits())));
            } else {
                outpost->addUnits(vessel->getUnits());
            }

            if(vessel->controlsSpecialist(SpecialistType::INSPECTOR) && vessel->getOwnerID() == outpost->getOwnerID()) outpost->rechargeShield();

            outpost->addSpecialists(vessel->removeSpecialists());
            setFriendly();
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

            bool vesselWins = (outpost->getUnits() + outpost->getShield() < vessel->getUnits()) || (outpost->getUnits() + outpost->getShield() == vessel->getUnits() && outpost->getSpecialists().size() < vessel->getSpecialists().size());

            setVictor(vesselWins ? vessel->getOwnerID() : outpost->getOwnerID());

            Player* vesselOwner = vessel->getOwner();
            Player* outpostOwner = outpost->getOwner();

            defeatSpecialistPhase(game);
            victorySpecialistPhase(game);
            postCombatSpecialistPhase(game);

            if(!vessel->isDeleted() && !outpost->isDeleted()) {
                outpost->addSpecialists(vessel->removeSpecialists());

                if(vesselWins) {
                    if(vesselOwner) vesselOwner->addOutpost(outpost);
                    else if(outpostOwner) outpostOwner->removeOutpost(outpost);
                    
                    outpost->addUnits(vessel->getUnits() - outpost->getUnits());
                }
            }

            for(const auto& pair : game->getPlayers()) {
                if(!pair.second->controlsSpecialist(SpecialistType::QUEEN)) {
                    pair.second->setDefeated(game);
                }
            }

            if(outpost->controlsSpecialist(SpecialistType::INSPECTOR)) outpost->rechargeShield();
        }

        if(outpost->controlsSpecialist(SpecialistType::HYPNOTIST)) {
            for(Specialist* s : outpost->getSpecialists()) {
                outpost->getOwner()->addSpecialist(s);
            }
        }

        if (outpost->controlsSpecialist(SpecialistType::DIPLOMAT)) {
            for (auto pair : game->getOutposts()) {
                Outpost* other = pair.second;
                if ((outpost->distance(other->getPosition()) <= outpost->getSonarRange()) && (other->getOwnerID() != outpost->getOwnerID())) {
                    std::list<Specialist*> specialists;

                    for (Specialist* s : other->getSpecialists()) {
                        if (s->getOwnerID() == outpost->getOwnerID()) {
                            specialists.push_front(s);
                        }
                    }

                    if(!specialists.empty()){
                        game->addEvent(new ReleaseEvent(nullptr, getTimestamp(), specialists, other));
                    }
                }
            }
        }
        
        std::unordered_map<unsigned int, std::list<Specialist*>> specialist_groups;
        for(Specialist* s : outpost->getSpecialists()){
            if(s->getOwnerID() != outpost->getOwnerID()){
               for (auto pair : game->getOutposts()) {
                  Outpost* other = pair.second;
                  if (other->controlsSpecialist(SpecialistType::DIPLOMAT) &&s->getOwnerID()== other->getOwnerID() && (outpost->distance(other->getPosition()) <= other->getSonarRange())) {
                     specialist_groups[s->getOwnerID()].push_front(s);
                     break;
                  }
               }
            }
        }
        for (const auto &p : specialist_groups) {
            game->addEvent(new ReleaseEvent(nullptr, getTimestamp(), p.second, outpost));
        }

        game->removeVessel(vessel);
    }
};

#endif