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
	Game game(start, start + 48 * 60 * 60, players, 42083, true);
	std::list<int> specialists;
	specialists.push_back(4);
	game.addOrder(new SendOrder(getTime(), 0, 20, specialists, 3, 7, -1));
	specialists = std::list<int>();
	specialists.push_back(8);
	game.addOrder(new SendOrder(getTime(), 2, 20, specialists, 7, 3, -1));
	specialists = std::list<int>();
	specialists.push_back(6);
	game.addOrder(new SendOrder(getTime() + 1, 1, 10, specialists, 5, 12, 0));
	game.run();
	
	std::cout << "Rejected: ";
	for(auto& o : game.getInvalid()) std::cout << o->getID() << ", ";
	if(game.getInvalid().empty()) std::cout << "none";
	std::cout << std::endl;
	std::cout << game.getOutpost(3)->getOwner()->getName() << " owns " << game.getOutpost(3)->getUnits() << " at (" << game.getOutpost(3)->getPosition().getX() << ", " << game.getOutpost(3)->getPosition().getY() << ")" << std::endl;
	std::cout << game.getOutpost(5)->getOwner()->getName() << " owns " << game.getOutpost(5)->getUnits() << " at (" << game.getOutpost(5)->getPosition().getX() << ", " << game.getOutpost(5)->getPosition().getY() << ")" << std::endl;
	std::cout << game.getOutpost(7)->getOwner()->getName() << " owns " << game.getOutpost(7)->getUnits() << " at (" << game.getOutpost(7)->getPosition().getX() << ", " << game.getOutpost(7)->getPosition().getY() << ")" << std::endl;
	std::cout << "Elapsed: " << difftime(game.getTime(), start)/3600 << " hours" << std::endl;
	std::shared_ptr<Game> cachedState = game.lastState(getTime() + 60 * 60 * 2);
	Point pos = cachedState->getVessel(14)->getPositionAt(cachedState->getDimensions(), getTime() + 60 * 60 * 2 - cachedState->getTime());
	std::cout << "Vessel 5 targeting vessel 3 at 2 hours in: "<< pos.getX() << ", " << pos.getY() << std::endl;
}


int main(int argc, char* argv[]) {
	run();
}