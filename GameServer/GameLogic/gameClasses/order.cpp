#include "order.h"
#include "game_object.h"

int Order::counter = 0;

/* Since IDs are assigned by order of creation, this function ensures that the player refers to the same
** objects regardless of whether new objects were created in the meantime.
** All opponent orders referring to objects that have not been created yet have IDs incremented to reflect
** the unanticipated creation of an object. This order must be an order that the opponent could not see
** when scheduling their order (hence the reference ID).
*/
void Order::updateOrders(const std::multiset<Order*, OrderOrder> &orders) const {
    for(auto &order : orders) {
        if(getID() > order->getReferenceID() && order->getSenderID() != getSenderID()) order->adjustIDs(GameObject::getIDCounter());
    }
}

bool OrderOrder::operator()(Order* lhs, Order* rhs) const 
{
    double diff = difftime(lhs->getTimestamp(), rhs->getTimestamp());
    return diff == 0 ? lhs->getID() < rhs->getID() : diff < 0;
}