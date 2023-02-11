#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <cstdlib>
#include "player.h"

class GameObject
{
private:
    int ID;

    bool stale;

    bool deleted;

public:
    GameObject() : ID(-1), stale(true), deleted(false) {}
    GameObject(int ID) : ID(ID), stale(true), deleted(false) {}

    int getID() const { return ID; }
    void setID(int ID) { this->ID = ID; }
    
    bool needsRefresh() const { return stale; }
    void setRefresh(bool refresh) { stale = refresh; }
    
    void remove() { deleted = true; }
    bool isDeleted() const { return deleted; }
};

#endif