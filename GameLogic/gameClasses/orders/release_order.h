#ifndef _RELEASE_ORDER_H_
#define _RELEASE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../events/release_event.h"

class ReleaseOrder : public Order
{
private:
    int specialistID;

public:
    ReleaseOrder(){};
    ReleaseOrder(int id, double timestamp, int senderID, int specialistID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), specialistID(specialistID) {}
    
    Order* copy() override { return new ReleaseOrder(*this); }
    
    int objIDDisplacement() override { return 1; }

    void adjustIDs(int createdID, int amount) override {
        if(specialistID >= createdID) specialistID += amount;
    }

    Event* converted(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost()) return nullptr;
        
        Specialist* s = game->getSpecialist(specialistID);

        if(!s || !s->getContainer() || s->getContainer()->getOwnerID() != getSenderID() || s->getOwnerID() == getSenderID()) return nullptr;

        Outpost* o = dynamic_cast<Outpost*>(s->getContainer());

        if(!o) return nullptr;

        if(!game->hasPlayer(s->getOwnerID()) || game->getPlayer(s->getOwnerID())->getOutposts().empty()) return nullptr;

        setDescription(std::string("Release ") + s->typeAsString() + std::string(" from ") + o->getName());

        return new ReleaseEvent(this, getTimestamp(), s, o);
    }

    std::string getType() const override { return "Release"; }
};

#endif