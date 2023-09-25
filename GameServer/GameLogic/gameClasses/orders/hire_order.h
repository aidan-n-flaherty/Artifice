#ifndef _HIRE_ORDER_H_
#define _HIRE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../events/hire_event.h"

class HireOrder : public Order
{
private:
    int specialistTypeID;

public:
    HireOrder(){};
    HireOrder(double timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}
    HireOrder(int id, double timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) return nullptr;
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return nullptr;
        }

        bool canHire = false;
        for(SpecialistType option : Specialist::baseHires()) {
            if(option == t) {
                canHire = true;
                break;
            }
        }
        if(!canHire) return nullptr;

        updateOrders(game->getOrders());

        return new HireEvent(this, getTimestamp(), player, new Specialist(t));
    }
};

#endif