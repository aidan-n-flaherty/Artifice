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
    ReleaseOrder(double timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistID(specialistID) {}
    ReleaseOrder(int id, double timestamp, int senderID, int specialistID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistID(specialistID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost()) return nullptr;
        
        Specialist* s = game->getSpecialist(specialistID);

        if(!s || !s->getContainer() || s->getContainer()->getOwnerID() != getSenderID() || s->getOwnerID() == getSenderID()) return nullptr;

        Outpost* o = dynamic_cast<Outpost*>(s->getContainer());

        if(!o) return nullptr;

        if(!game->hasPlayer(s->getOwnerID()) || game->getPlayer(s->getOwnerID())->getOutposts().empty()) return nullptr;

        updateOrders(game, game->getOrders());

        return new ReleaseEvent(this, getTimestamp(), s, o);
    }

    std::string getType() override { return "Release"; }
};

#endif