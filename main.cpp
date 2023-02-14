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
	std::list<int> params;
	params.push_back(0);
	params.push_back(2);
	params.push_back(20);
	game.addOrder(Order(getTime(), 123456, OrderType::SEND, params));
	params.clear();
	params.push_back(2);
	params.push_back(0);
	params.push_back(20);
	game.addOrder(Order(getTime(), 987654, OrderType::SEND, params));
	params.clear();
	params.push_back(1);
	params.push_back(3);
	params.push_back(10);
	game.addOrder(Order(getTime() + 1, 654321, OrderType::SEND, params));
	game.run();
	std::cout << "Rejected: " << game.getInvalid().size() << std::endl;
	std::cout << game.getOutpost(0)->getUnits() << ", " << game.getOutpost(1)->getUnits() << ", " << game.getOutpost(2)->getUnits() << std::endl;
	std::cout << "Elapsed: " << difftime(game.getTime(), start)/3600 << " hours" << std::endl;
	std::shared_ptr<Game> cachedState = game.lastState(getTime() + 60 * 60 * 2);
	Point pos = cachedState->getVessel(5)->getPositionAt(cachedState->getDimensions(), getTime() + 60 * 60 * 2 - cachedState->getTime());
	std::cout << pos.getX() << ", " << pos.getY() << std::endl;
}


int main(int argc, char* argv[]) {
	run();
}