#ifndef _POSSESSABLE_H_
#define _POSSESSABLE_H_

#include <cstdlib>
#include <memory>

class Game;

class Player;

class Possessable
{
private:
    Player* owner = nullptr;

public:
    Possessable() {}
    Possessable(Player* owner) : owner(owner) {}

    virtual void updatePointers(Game* game);

    bool hasOwner() const { return owner; }

    int getOwnerID() const;
    Player* getOwner() const { return owner; }

    virtual void setOwner(Player* owner) { this->owner = owner; }
};

#endif