#ifndef _PROMOTE_ORDER_H_
#define _PROMOTE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../events/promote_event.h"

class PromoteOrder : public Order
{
private:
    int specialistID;

    int specialistTypeID;

public:
    PromoteOrder(){};
    PromoteOrder(time_t timestamp, int senderID, int specialistID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistID(specialistID) {}
    PromoteOrder(int id, time_t timestamp, int senderID, int specialistID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistID(specialistID) {}

    std::shared_ptr<Event> convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        std::shared_ptr<Player> player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) return nullptr;
        
        std::shared_ptr<Specialist> specialist = game->getSpecialist(specialistID);

        if(specialist->getOwnerID() != getSenderID()) return nullptr;
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return nullptr;
        }

        return std::shared_ptr<Event>(new PromoteEvent(getTimestamp(), player, specialist, t));
    }
};

#endif