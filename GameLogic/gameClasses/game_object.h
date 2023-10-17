#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <cstdlib>
#include "order.h"
#include "game_settings.h"

class Game;

class GameObject
{
private:
    Order* originatingOrder;

    GameSettings* settings;

    const unsigned int ID = 0;

    bool stale = false;

    bool deleted = false;

public:
    GameObject(unsigned int ID, GameSettings* settings) : ID(ID), settings(settings), stale(true), deleted(false) {}

    unsigned int getID() const { return ID; }

    bool needsRefresh() const { return stale; }
    void setRefresh(bool refresh) { stale = refresh; }
    
    void remove() { deleted = true; }
    bool isDeleted() const { return deleted; }
    
    void setOriginatingOrder(Order* o) { originatingOrder = o; }
    Order* getOriginatingOrder() { return originatingOrder; }

    const GameSettings* getSettings() const { return settings; }
};

#endif