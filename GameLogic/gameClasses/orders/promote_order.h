#ifndef _PROMOTE_ORDER_H_
#define _PROMOTE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <iostream>
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
    PromoteOrder(double timestamp, int senderID, int specialistID, int promoteID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistID(specialistID), specialistTypeID(promoteID) {}
    PromoteOrder(int id, double timestamp, int senderID, int specialistID, int promoteID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), specialistID(specialistID), specialistTypeID(promoteID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) {
            std::cout << "Not enough hires or player has lost" << std::endl;
            return nullptr;
        }

        if(!game->hasSpecialist(specialistID)) {
            std::cout << "Specialist does not exist" << std::endl;
            return nullptr;
        }
        
        Specialist* specialist = game->getSpecialist(specialistID);

        if(specialist->getOwnerID() != getSenderID()) {
            std::cout << "Does not own specialist" << std::endl;
            return nullptr;
        }
        
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

        return new PromoteEvent(this, getTimestamp(), specialist, t);
    }

    std::string getType() override { return "Promote"; }
};

#endif