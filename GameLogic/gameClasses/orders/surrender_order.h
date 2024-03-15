#ifndef _SURRENDER_ORDER_H_
#define _SURRENDER_ORDER_H_

#include <cstdlib>
#include <cmath>
#include <list>
#include <ctime>
#include "../event.h"
#include "../game.h"
#include "../game_settings.h"
#include "../events/surrender_event.h"

class SurrenderOrder : public Order
{
public:
    SurrenderOrder(){};
    SurrenderOrder(double timestamp, int senderID, int referenceID) :
        Order(timestamp, senderID, referenceID) {}
    SurrenderOrder(int id, double timestamp, int senderID, int referenceID) :
        Order(id, timestamp, senderID, referenceID) {}

    Event* convert(Game* game) override {
        if(!game->hasPlayer(getSenderID())) return nullptr;
        
        Player* player = game->getPlayer(getSenderID());
        
        if(player->hasLost()) return nullptr;

        return new SurrenderEvent(this, getTimestamp(), player);
    }

    std::string getType() override { return "Surrender"; }
};

#endif