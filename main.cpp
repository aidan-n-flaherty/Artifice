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
	std::list<Player> players;
	players.push_back(Player("Bob", 123456));
	players.push_back(Player("Joe", 654321));
	players.push_back(Player("Steve", 987654));

	time_t start = getTime();
	Game game(start, players, 105980);
	std::multiset<Order> orders;
	std::list<int> params;
	params.push_back(0);
	params.push_back(2);
	params.push_back(20);
	orders.insert(Order(getTime(), 123456, SEND, params));
	std::list<Order> invalid;
	game.run(orders, invalid);
	std::cout << invalid.size() << std::endl;
	std::cout << game.getOutpost(0).getUnits() << ", " << game.getOutpost(1).getUnits() << ", " << game.getOutpost(2).getUnits() << std::endl;
	std::cout << "Elapsed: " << difftime(game.getTime(), start)/3600 << " hours" << std::endl;
}


int main(int argc, char* argv[]) {
	run();
}