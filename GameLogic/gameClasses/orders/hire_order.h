#ifndef _HIRE_ORDER_H_
#define _HIRE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <iostream>
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
    HireOrder(int id, double timestamp, int senderID, int specialistTypeID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), specialistTypeID(specialistTypeID) {}

    Order* copy() override { return new HireOrder(*this); }

    int objIDDisplacement() override {
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
        } catch(...) {
            return 0;
        }

        return Specialist::hireAmount(t);
    }

    Event* converted(Game* game) override {
        std::cout << "Hiring..." << getSenderID() << std::endl;
        if(!game->hasPlayer(getSenderID())) {
            std::cout << "ORDER ERROR: Cannot find sender" << std::endl;
            return nullptr;
        }

        Player* player = game->getPlayer(getSenderID());

        std::cout << player->getName() << std::endl;
        
        if(player->hasLost() || player->getHires() <= 0) {
            std::cout << "ORDER ERROR: Insufficient hires" << std::endl;
            return nullptr;
        }
        
        SpecialistType t;

        try {
            t = SpecialistType(specialistTypeID);
            std::cout << "Hiring " << Specialist::typeAsString(t) << std::endl;
        } catch(...) {
            std::cout << "ORDER ERROR: Cannot parse specialist type " << specialistTypeID << std::endl;
            return nullptr;
        }

        if(!dynamic_cast<Outpost*>(player->getSpawnLocation())) {
            std::cout << "ORDER ERROR: Queen not at outpost" << std::endl;
            return nullptr;
        }

        if(player->getSpawnLocation()->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: player does not own Queen's container" << std::endl;
            return nullptr;
        }

        if(game->getSettings()->specialistBans.find(t) != game->getSettings()->specialistBans.end()) {
            std::cout << "ORDER ERROR: banned specialist" << std::endl;
            return nullptr;
        }

        bool canHire = false;
        for(SpecialistType option : Specialist::baseHires()) {
            if(option == t) {
                canHire = true;
                break;
            }
        }
        if(!canHire) {
            std::cout << "ORDER ERROR: Not in base hires" << std::endl;
            return nullptr;
        }

        setDescription(std::string("Hire ") + Specialist::typeAsString(t));

        std::cout << "Hired " << game->getObjCounter() << std::endl;
        return new HireEvent(this, getTimestamp(), player, t);
    }

    std::string getType() const override { return "Hire"; }
};

#endif