#ifndef _MINE_ORDER_H_
#define _MINE_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../game_settings.h"
#include "../events/mine_event.h"

class MineOrder : public Order
{
private:
    int outpostID;

public:
    MineOrder(){};
    MineOrder(double timestamp, int senderID, int outpostID, int referenceID) :
        Order(timestamp, senderID, referenceID), outpostID(outpostID) {}
    MineOrder(int id, double timestamp, int senderID, int outpostID, int referenceID) :
        Order(id, timestamp, senderID, referenceID), outpostID(outpostID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID()) || !game->hasOutpost(outpostID)) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost()) return nullptr;

        Outpost* outpost = game->getOutpost(outpostID);

        if(outpost->getUnits() < player->getMineCost() || outpost->getType() == OutpostType::MINE || outpost->getType() == OutpostType::BROKEN || outpost->getOwnerID() != getSenderID()) return nullptr;

        return new MineEvent(this, getTimestamp(), outpost);
    }

    std::string getType() override { return "Mine"; }
};

#endif