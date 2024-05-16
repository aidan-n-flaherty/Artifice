#ifndef _HIRE_EVENT_H_
#define _HIRE_EVENT_H_

#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "../event.h"
#include "../gameObjects/player.h"
#include "../gameObjects/specialist.h"
#include "../game.h"
#include "release_event.h"

class HireEvent : public Event
{
private:
    Player* owner;
    std::list<Specialist*> specialists;

public:
    HireEvent(){};
    HireEvent(Order* o, double timestamp, Player* owner, std::list<Specialist*> specialists) : Event(o, timestamp), owner(owner), specialists(specialists) {}
    
    Event* copy() override { return new HireEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        for(Specialist* &specialist : specialists) specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        for(Specialist* specialist : specialists) {
            specialist->setOriginatingOrder(getOriginatingOrder());
            game->addSpecialist(specialist);
            owner->addSpecialist(specialist);
            PositionalObject* spawn = owner->getSpawnLocation();
            if(spawn) spawn->addSpecialist(specialist);
            Outpost* outpost = dynamic_cast<Outpost*>(spawn);
            if (outpost && specialist->getType() == SpecialistType::DIPLOMAT) {
                for (auto pair : game->getOutposts()) {
                    Outpost* other = pair.second;
                    if ((outpost->distance(other->getPosition()) <= outpost->getSonarRange()) && (other->getOwnerID() != outpost->getOwnerID())) {
                        std::list<Specialist*> specialistsToRelease;
                        
                        for (Specialist* s : other->getSpecialists()) {
                            if (s->getOwnerID() == outpost->getOwnerID()) {
                                specialistsToRelease.push_front(s);
                            }
                        }

                        if (!specialistsToRelease.empty()) {
                            game->addEvent(new ReleaseEvent(nullptr, getTimestamp(), specialistsToRelease, other));
                        }
                    }
                }
            }
        }
        
        owner->removeHire();
    }
};

#endif