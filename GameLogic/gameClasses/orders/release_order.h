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
        Order(timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}
    ReleaseOrder(int id, double timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) return nullptr;
        
        Specialist* s = game->getSpecialist(specialistID);

        if(!s || !s->getContainer() || s->getContainer()->getOwnerID() != getSenderID() || s->getOwnerID() == getSenderID()) return nullptr;

        if(!game->hasPlayer(s->getOwnerID()) || game->getPlayer(s->getOwner())->getOutposts().empty()) return nullptr;

        updateOrders(game, game->getOrders());

        return new ReleaseEvent(this, getTimestamp(), s);
    }

    std::string getType() override { return "Hire"; }
};

#endif