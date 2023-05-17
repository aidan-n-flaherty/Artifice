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
    HireOrder(time_t timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}
    HireOrder(int id, time_t timestamp, int senderID, int specialistTypeID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistTypeID(specialistTypeID) {}

    std::shared_ptr<Event> convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        std::shared_ptr<Player> player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) return nullptr;
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return nullptr;
        }

        updateOrders(game->getOrders());

        return std::shared_ptr<Event>(new HireEvent(getTimestamp(), player, std::shared_ptr<Specialist>(new Specialist(t))));
    }
};

#endif