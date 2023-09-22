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
    PromoteOrder(double timestamp, int senderID, int specialistID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistID(specialistID) {}
    PromoteOrder(int id, double timestamp, int senderID, int specialistID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistID(specialistID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) return nullptr;
        
        Specialist* specialist = game->getSpecialist(specialistID);

        if(specialist->getOwnerID() != getSenderID()) return nullptr;
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return nullptr;
        }

        bool canPromote = false;
        for(SpecialistType option : specialist->promotionOptions()) {
            if(option == t) {
                canPromote = true;
                break;
            }
        }
        if(!canPromote) return nullptr;

        return new PromoteEvent(getTimestamp(), specialist, t);
    }
};

#endif