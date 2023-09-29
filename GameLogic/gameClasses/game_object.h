#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <cstdlib>
#include "order.h"

class Game;

class GameObject
{
private:
    Order* originatingOrder;
    
    static unsigned int counter;

    const unsigned int ID;

    bool stale;

    bool deleted;

public:
    GameObject() : ID(counter++), stale(true), deleted(false) {}

    unsigned int getID() const { return ID; }

    bool needsRefresh() const { return stale; }
    void setRefresh(bool refresh) { stale = refresh; }
    
    void remove() { deleted = true; }
    bool isDeleted() const { return deleted; }

    static int getIDCounter() { return counter; }
    static void resetCounter(int num) { counter = num; }
    
    void setOriginatingOrder(Order* o) { originatingOrder = o; }
    Order* getOriginatingOrder() { return originatingOrder; }
};

#endif