#include "event.h"
#include "game.h"
#include "gameObjects/specialist.h"
#include <list>

int Event::counter = 0;

void Event::updatePointers(Game* game) {
   if(originatingOrder) originatingOrder = game->getOrder(originatingOrder->getID());
}