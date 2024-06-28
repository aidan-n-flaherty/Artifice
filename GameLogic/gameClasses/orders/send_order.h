#ifndef _SEND_ORDER_H_
#define _SEND_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include <set>
#include <iostream>
#include "../event.h"
#include "../game.h"
#include "../events/send_event.h"

class SendOrder : public Order
{
private:
    int numUnits;

    int originID;
    int targetID;
    std::list<int> specialistIDs;

public:
    SendOrder(){};
    SendOrder(double timestamp, int senderID, int numUnits, std::list<int> specialistIDs, int originID, int targetID, int referenceID, bool canceled) :
        Order(timestamp, senderID, referenceID), numUnits(numUnits), specialistIDs(specialistIDs),
        originID(originID), targetID(targetID) {}
    SendOrder(int id, double timestamp, int senderID, int numUnits, std::list<int> specialistIDs, int originID, int targetID, int referenceID, bool canceled) :
        Order(id, timestamp, senderID, referenceID, canceled), numUnits(numUnits), specialistIDs(specialistIDs),
        originID(originID), targetID(targetID) {}

    Order* copy() override { return new SendOrder(*this); }

    void adjustIDs(int createdID, int amount) override {
        if(originID >= createdID) originID += amount;
        if(targetID >= createdID) targetID += amount;
        for(int& specialistID : specialistIDs) if(specialistID >= createdID) specialistID += amount;
    }

    int objIDDisplacement() override { return 1; }

    int getUnits() { return numUnits; }

    void setUnits(int units) { numUnits = units; }

    int getOriginID() { return originID; }

    int getTargetID() { return targetID; }

    std::list<int> getSpecialistIDs() { return specialistIDs; }

    void setSpecialistIDs(const std::list<int>& specialistIDs) { this->specialistIDs = specialistIDs; }

    Event* converted(Game* game) override {
        if(!game->hasOutpost(originID)) {
            std::cout << "ORDER ERROR: nonexistent origin" << std::endl;
            return nullptr;
        }

        Outpost* outpost = game->getOutpost(originID);

        if(!game->hasPosObject(targetID)) {
            std::cout << "ORDER ERROR: nonexistent target" << std::endl;
            return nullptr;
        }

        PositionalObject* target = game->getPosObject(targetID);

        Outpost* targetOutpost = dynamic_cast<Outpost*>(target);

        if(targetOutpost) {
            setDescription(std::string("Send ") + std::to_string(numUnits) + std::string(" units from ") + outpost->getName() + " to " + targetOutpost->getName());
        } else {
            setDescription(std::string("Send ") + std::to_string(numUnits) + std::string(" units from ") + outpost->getName() + " to an enemy submarine");
        }

        if(!specialistIDs.empty()) setDescription(getDescription() + " with " + std::to_string(specialistIDs.size()) + " specialist" + (specialistIDs.size() == 1 ? "" : "s"));

        if(!game->hasPlayer(getSenderID()) || game->getPlayer(getSenderID())->hasLost()) {
            std::cout << "ORDER ERROR: player has lost" << std::endl;
            return nullptr;
        }

        if(originID == targetID) {
            std::cout << "ORDER ERROR: cannot send to self" << std::endl;
            return nullptr;
        }

        if(outpost->getOwnerID() != getSenderID()) {
            std::cout << "ORDER ERROR: outpost " << outpost->getID() << " is not owned by " << getSenderID() << std::endl;
            return nullptr;
        }

        if(!outpost->canRemoveSpecialists(specialistIDs) || !outpost->canRemoveUnits(numUnits)) {
            for(int i : specialistIDs) std::cout << i << " ";
            std::cout << std::endl;
            for(Specialist* s : outpost->getOwner()->getSpecialists()) std::cout << s->getID() << " ";
            std::cout << std::endl;
            std::cout << "ORDER ERROR: insufficient specialists or units" << std::endl;

            return nullptr;
        }

        // TODO: add check that specialist count is 3 or less

        std::list<Specialist*> specialists;
        for(int specialistID : specialistIDs) specialists.push_back(game->getSpecialist(specialistID));

        if(specialists.empty() && numUnits <= 0) {
            std::cout << "ORDER ERROR: cannot send empty vessel" << std::endl;
            return nullptr;
        }

        // only pirates may target vessels
        if(game->hasVessel(targetID)) {
            bool hasPirate = false;

            for(Specialist* specialist : specialists) {
                if(specialist->getType() == SpecialistType::PIRATE) {
                    hasPirate = true;
                    break;
                }
            }

            if(!hasPirate) {
                std::cout << "ORDER ERROR: cannot target vessel without pirate" << std::endl;
                return nullptr;
            }
        }

        return new SendEvent(this, getTimestamp(), numUnits, specialists, outpost, target);
    }

    std::string getType() const override { return "Send"; }
};

#endif