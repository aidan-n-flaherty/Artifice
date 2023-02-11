#ifndef _POSSESSABLE_H_
#define _POSSESSABLE_H_

#include <cstdlib>
#include "player.h"

class Possessable
{
private:
    Player* owner;

public:
    Possessable() {}
    Possessable(Player* owner) : owner(owner) {}

    bool hasOwner() const { return owner != nullptr; }

    int getOwnerID() const { return owner->getID(); }
    Player* getOwner() const { return owner; }

    void setOwner(Player* owner) { this->owner = owner; }
};

#endif