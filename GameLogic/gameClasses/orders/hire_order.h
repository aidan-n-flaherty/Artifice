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

        if(!dynamic_cast<Outpost*>(player->getSpawnLocation())) return nullptr;

        if(player->getSpawnLocation()->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: player does not own Queen's container" << std::endl;
            return nullptr;
        }

        if(game->getSettings()->specialistBans.find(t) != game->getSettings()->specialistBans.end()) return nullptr;

        bool canHire = false;
        for(SpecialistType option : Specialist::baseHires()) {
            if(option == t) {
                canHire = true;
                break;
            }
        }
        if(!canHire) return nullptr;

        updateOrders(game, game->getOrders());

        std::list<Specialist*> specialists;
        for(int i = 0; i < Specialist::hireAmount(t); i++) {
            specialists.push_back(new Specialist(game->incrementObjCounter(), game->getSettings(), t));
        }

        return new HireEvent(this, getTimestamp(), player, specialists);
    }

    std::string getType() override { return "Hire"; }
};

#endif