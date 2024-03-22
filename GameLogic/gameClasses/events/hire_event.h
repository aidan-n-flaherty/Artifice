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
    Specialist* specialist;

public:
    HireEvent(){};
    HireEvent(Order* o, double timestamp, Player* owner, Specialist* specialist) : Event(o, timestamp), owner(owner), specialist(specialist) {}
    
    Event* copy() override { return new HireEvent(*this); }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        owner = game->getPlayer(owner->getID());
        specialist = game->getSpecialist(specialist->getID());
    }

    void run(Game* game) override {
        specialist->setOriginatingOrder(getOriginatingOrder());
        game->addSpecialist(specialist);
        owner->addSpecialist(specialist);
        owner->removeHire();

        PositionalObject* spawn = owner->getSpawnLocation();
        if(spawn) spawn->addSpecialist(specialist);
        Outpost* outpost = dynamic_cast<Outpost*>(spawn);
        if (outpost && specialist->getType() == SpecialistType::DIPLOMAT) {
            for (auto pair : game->getOutposts()) {
                std::list<Specialist*> specialists;
                Outpost* other = pair.second;
                if ((outpost->distance(other->getPosition()) <= outpost->getSonarRange()) && (other->getOwnerID() != outpost->getOwnerID())) {
                    std::list<Specialist*> specialists;
                    for (Specialist* s : other->getSpecialists()) {
                        if (s->getOwnerID() == outpost->getOwnerID()) {
                            specialists.push_front(s);
                        }
                    }
                }
                if (!specialists.empty()) {
                    std::cout << "diplomat event added" << std::endl;
                    game->addEvent(new ReleaseEvent(nullptr, getTimestamp(), specialists, other));
                }
            }
        }
    }
};

#endif