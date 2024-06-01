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
    SpecialistType type;

public:
    HireEvent(){};
    HireEvent(Order* o, double timestamp, Player* owner, SpecialistType type) : Event(o, timestamp), owner(owner), type(type) {}
    
    Event* copy() override { return new HireEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
    }

    void run(Game* game) override {
        std::list<Specialist*> specialists;
        for(int i = 0; i < Specialist::hireAmount(type); i++) {
            specialists.push_back(new Specialist(game->incrementObjCounter(), game->getSettings(), type));
        }

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