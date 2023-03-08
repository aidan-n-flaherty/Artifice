#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <cstdlib>
#include "player.h"

class Game;

class GameObject
{
private:
    static int counter;

    const int ID;

    bool stale;

    bool deleted;

public:
    GameObject() : ID(counter++), stale(true), deleted(false) {}

    int getID() const { return ID; }

    bool needsRefresh() const { return stale; }
    void setRefresh(bool refresh) { stale = refresh; }
    
    void remove() { deleted = true; }
    bool isDeleted() const { return deleted; }

    static int getIDCounter() { return counter; }
};

#endif