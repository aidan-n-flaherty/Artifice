#include "order.h"
#include "game_object.h"
#include "game.h"

#include <algorithm>

int Order::counter = 0;

/* Since IDs are assigned by order of creation, this function ensures that the player refers to the same
** objects regardless of whether new objects were created in the meantime.
** All opponent orders referring to objects that have not been created yet have IDs incremented to reflect
** the unanticipated creation of an object. This order must be an order that the opponent could not see
** when scheduling their order (hence the reference ID).
*/
void Order::updateOrders(Game* game, const std::multiset<Order*, OrderOrder> &orders) const {
    for(auto &order : orders) {
        // if the order's reference order has already been run, then this order must have not been seen
        if(std::find_if(orders.begin(), orders.end(), [&order](Order* o) {
            return o->getID() == order->getReferenceID();
        }) == orders.end() && order->getSenderID() != getSenderID()) order->adjustIDs(game->getObjCounter());
    }
}

bool OrderOrder::operator()(Order* lhs, Order* rhs) const 
{
    double diff = lhs->getTimestamp() - rhs->getTimestamp();
    return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
}