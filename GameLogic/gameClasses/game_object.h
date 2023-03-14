#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <cstdlib>

class Game;

class GameObject
{
private:
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
};

#endif