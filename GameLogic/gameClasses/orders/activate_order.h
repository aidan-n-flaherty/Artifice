#ifndef _ACTIVATE_ORDER_H_
#define _ACTIVATE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <iostream>
#include "../event.h"
#include "../game.h"
#include "../events/activate_event.h"

class ActivateOrder : public Order
{
private:
    int specialistID;

public:
    ActivateOrder(){};
    ActivateOrder(double timestamp, int senderID, int specialistID, int referenceID) :
        Order(timestamp, senderID, referenceID), specialistID(specialistID) {}
    ActivateOrder(int id, double timestamp, int senderID, int specialistID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), specialistID(specialistID) {}

    Order* copy() override { return new ActivateOrder(*this); }

    void adjustIDs(int createdID, int amount) override {
        if(specialistID >= createdID) specialistID += amount;
    }

    Event* converted(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;

        if(!game->hasSpecialist(specialistID)) {
            std::cout << "ORDER ERROR: specialist does not exist" << std::endl;
            return nullptr;
        }
        
        Specialist* specialist = game->getSpecialist(specialistID);

        setDescription(std::string("Activate ") + specialist->typeAsString() + std::string("'s ability"));
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost()) {
            std::cout << "ORDER ERROR: player has lost" << std::endl;
            return nullptr;
        }

        if(specialist->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: does not own specialist" << std::endl;
            return nullptr;
        }

        if(specialist->getContainer()->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: player does not own specialist's container" << std::endl;
            return nullptr;
        }

        if(specialist->getType() == SpecialistType::DETONATOR) {
            std::cout << "Activated Detonator" << std::endl;
        } else {
            std::cout << "ORDER ERROR: not activatable" << std::endl;
            return nullptr;
        }

        return new ActivateEvent(this, getTimestamp(), specialist);
    }

    std::string getType() const override { return "Activate"; }
};

#endif