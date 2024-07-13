#ifndef _RETREAT_EVENT_H_
#define _RETREAT_EVENT_H_

#include <cstdlib>
#include "../event.h"
#include "../game.h"

class RetreatEvent : public Event
{
private:
    Vessel* vessel;
public:
    RetreatEvent(){};
    RetreatEvent(Order* o, double timestamp, Vessel* vessel) :
        Event(o, timestamp), vessel(vessel) {}
    
    Event* copy() override { return new RetreatEvent(*this); }

    bool referencesObject(int id) const override { return vessel->getID() == id; }

    void updatePointers(Game *game) override {
        Event::updatePointers(game);
        vessel = game->getVessel(vessel->getID());
    }

    void run(Game* game) override {
        if(game->ignoreVessel(vessel->getID(), getTimestamp())) {
            setDisabled(true);
            return;
        }

        vessel->setOriginatingOrder(getOriginatingOrder());
        Outpost* targetOutpost = dynamic_cast<Outpost*>(vessel->getTarget());
        if(vessel->getOrigin() && targetOutpost) {
            vessel->setOrigin(targetOutpost);
        } else vessel->setOrigin(nullptr);
        
        vessel->setTarget(vessel->getReturnOutpost());
        vessel->setSpeedModifier(1.0);
        if(vessel->hasOwner()) vessel->getOwner()->addVesselPath(getTimestamp(), vessel->getPosition(), vessel->getTargetPos());

        for(auto& pair : game->getVessels()) {
            if(pair.second->getTargetID() == vessel->getID()) {
                pair.second->setOrigin(nullptr);
            }
        }
    }
};

#endif