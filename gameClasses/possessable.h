#ifndef _POSSESSABLE_H_
#define _POSSESSABLE_H_

#include <cstdlib>
#include "player.h"

class Game;

class Possessable
{
private:
    std::shared_ptr<Player> owner;

public:
    Possessable() {}
    Possessable(std::shared_ptr<Player> owner) : owner(owner) {}
    Possessable(const std::shared_ptr<Possessable> other, Game* game);
    virtual void updatePointers(Game* game);

    bool hasOwner() const { return owner != nullptr; }

    int getOwnerID() const { return owner->getID(); }
    std::shared_ptr<Player> getOwner() const { return owner; }

    void setOwner(std::shared_ptr<Player> owner) { this->owner = owner; }
};

#endif