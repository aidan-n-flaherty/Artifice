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
    PromoteOrder(int id, double timestamp, int senderID, int specialistID, int promoteID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), specialistID(specialistID), specialistTypeID(promoteID) {}

    Order* copy() override { return new PromoteOrder(*this); }

    Event* converted(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost() || player->getHires() <= 0) {
            std::cout << "ORDER ERROR: not enough hires or player has lost" << std::endl;
            return nullptr;
        }

        if(!game->hasSpecialist(specialistID)) {
            std::cout << "ORDER ERROR: specialist does not exist" << std::endl;
            return nullptr;
        }
        
        Specialist* specialist = game->getSpecialist(specialistID);

        if(specialist->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: does not own specialist" << std::endl;
            return nullptr;
        }

        if(specialist->getContainer()->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: player does not own specialist's container" << std::endl;
            return nullptr;
        }

        if(!dynamic_cast<Outpost*>(specialist->getContainer())) {
            std::cout << "ORDER ERROR: specialist not at outpost" << std::endl;
            return nullptr;
        }
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return nullptr;
        }

        if(game->getSettings()->specialistBans.find(t) != game->getSettings()->specialistBans.end()) return nullptr;

        bool canPromote = false;
        for(SpecialistType option : specialist->promotionOptions()) {
            if(option == t) {
                canPromote = true;
                break;
            }
        }
        if(!canPromote) return nullptr;

        setDescription(std::string("Promote ") + specialist->typeAsString() + std::string(" to ") + Specialist::typeAsString(t));

        return new PromoteEvent(this, getTimestamp(), specialist, t);
    }

    std::string getType() const override { return "Promote"; }
};

#endif