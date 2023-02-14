#include "player.h"
#include "gameObjects/specialist.h"
#include "game.h"

void Player::updatePointers(Game* game) {
    for(std::shared_ptr<Specialist> &a : this->specialists) a = game->getSpecialist(a->getID());
}

double Player::globalSpeed(){
    return 1;
}

int Player::attackPower(int numUnits){
    return numUnits;
}

bool Player::hasSpecialist(SpecialistType t) const {
    for(auto it = specialists.begin(); it != specialists.end(); ++it){
        if((*it)->getType() == t) {
            return true;
        }
    }

    return false;
}

bool Player::hasSpecialists(std::list<int> specialists) {
    int count = 0;

    for(std::shared_ptr<Specialist> specialist : this->specialists) {
        for(int id : specialists) {
            if(specialist->getID() == id){
                count++;
                
                if(count == specialists.size()) {
                    return true;
                }
            }
        }
    }

    return false;
}

