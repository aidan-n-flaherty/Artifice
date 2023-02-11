#include "player.h"
#include "gameObjects/specialist.h"

double Player::globalSpeed(){
    return 1;
}

int Player::attackPower(int numUnits){
    return numUnits;
}

bool Player::hasSpecialists(std::list<int> specialists) {
    int count = 0;

    for(Specialist* specialist : this->specialists) {
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