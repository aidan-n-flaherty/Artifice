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
	std::shared_ptr<Game> game = std::shared_ptr<Game>(new Game(123456, start, start + 48 * 60 * 60, players, 42083, true));
	std::list<int> specialists;
	specialists.push_back(4);
	/*game->addOrder(new SendOrder(start, 0, 20, specialists, 3, 7, -1));
	specialists = std::list<int>();
	specialists.push_back(8);
	game->addOrder(new SendOrder(start, 2, 20, specialists, 7, 3, -1));
	specialists = std::list<int>();
	specialists.push_back(6);*/
	game->run();
	//game.addOrder(new SendOrder(start + 1, 1, 10, specialists, 5, 12, 0));
	//int tmp[] { 10, 5, 12, 6};
	//game = game->processOrder("SEND", 2, 0, start + 1, 1, tmp, 4);

	int tmp[] { 1,5,3 };
	game = game->processOrder("SEND", 42, -1, start + 1, 1, tmp, 3);
	game->run();
	
	std::cout << "Rejected: ";
	for(auto& o : game->getInvalid()) std::cout << o->getID() << ", ";
	if(game->getInvalid().empty()) std::cout << "none";
	std::cout << std::endl;
	std::cout << game->getOutpost(3)->getOwner()->getName() << " owns " << game->getOutpost(3)->getUnits() << " at (" << game->getOutpost(3)->getPosition().getX() << ", " << game->getOutpost(3)->getPosition().getY() << ")" << std::endl;
	std::cout << game->getOutpost(5)->getOwner()->getName() << " owns " << game->getOutpost(5)->getUnits() << " at (" << game->getOutpost(5)->getPosition().getX() << ", " << game->getOutpost(5)->getPosition().getY() << ")" << std::endl;
	std::cout << game->getOutpost(7)->getOwner()->getName() << " owns " << game->getOutpost(7)->getUnits() << " at (" << game->getOutpost(7)->getPosition().getX() << ", " << game->getOutpost(7)->getPosition().getY() << ")" << std::endl;
	std::cout << "Elapsed: " << difftime(game->getTime(), start)/3600 << " hours" << std::endl;
	std::shared_ptr<Game> cachedState = game->lastState(start + 1);
	time_t nextState = game->nextState(start + 1);
	Point pos = cachedState->getVessel(14)->getPositionAt(10);
	std::cout << "Vessel 5 targeting vessel 3 at 10 seconds in: "<< pos.getX() << ", " << pos.getY() << std::endl;
}


int main(int argc, char* argv[]) {
	run();
}