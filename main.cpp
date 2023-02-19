#include <cstdlib>
#include <cmath>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>
#include <set>
#include "gameClasses/game.h"
#include "gameClasses/orders/send_order.h"

time_t getTime() {
	time_t rawtime;

	time (&rawtime);
	
	return rawtime;
}

void run() {
	std::map<int, std::string> players;
	players[123456] = "Bob";
	players[654321] = "Joe";
	players[987654] = "Steve";

	time_t start = getTime();
	Game game(start, players, 42083, true);
	std::list<int> specialists;
	specialists.push_back(1);
	game.addOrder(new SendOrder(getTime(), 123456, 20, specialists, 0, 4, -1));
	specialists.clear();
	specialists.push_back(5);
	game.addOrder(new SendOrder(getTime(), 987654, 20, specialists, 4, 0, -1));
	specialists.clear();
	game.addOrder(new SendOrder(getTime() + 1, 654321, 10, specialists, 2, 6, 0));
	game.run();
	std::cout << "Rejected: ";
	for(auto& o : game.getInvalid()) std::cout << o->getID() << ", ";
	if(game.getInvalid().empty()) std::cout << "none";
	std::cout << std::endl;
	std::cout << game.getOutpost(0)->getOwner()->getName() << " owns " << game.getOutpost(0)->getUnits() << " at (" << game.getOutpost(0)->getPosition().getX() << ", " << game.getOutpost(0)->getPosition().getY() << ")" << std::endl;
	std::cout << game.getOutpost(2)->getOwner()->getName() << " owns " << game.getOutpost(2)->getUnits() << " at (" << game.getOutpost(2)->getPosition().getX() << ", " << game.getOutpost(2)->getPosition().getY() << ")" << std::endl;
	std::cout << game.getOutpost(4)->getOwner()->getName() << " owns " << game.getOutpost(4)->getUnits() << " at (" << game.getOutpost(4)->getPosition().getX() << ", " << game.getOutpost(4)->getPosition().getY() << ")" << std::endl;
	std::cout << "Elapsed: " << difftime(game.getTime(), start)/3600 << " hours" << std::endl;
	std::shared_ptr<Game> cachedState = game.lastState(getTime() + 60 * 60 * 2);
	Point pos = cachedState->getVessel(8)->getPositionAt(cachedState->getDimensions(), getTime() + 60 * 60 * 2 - cachedState->getTime());
	std::cout << "Vessel 5 targeting vessel 3 at 2 hours in: "<< pos.getX() << ", " << pos.getY() << std::endl;
}


int main(int argc, char* argv[]) {
	run();
}