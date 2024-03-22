#include "game.h"
#include "gameWrapper.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <list>
#include <tuple>
#include <map>
#include <cstring>
#include <iostream>

cGameSettings initGameSettings() {
  GameSettings* settings = new GameSettings();
  return (void*)settings;
}

void freeGameSettings(cGameSettings settings) {
  GameSettings* s = (GameSettings*)settings;
  delete s;
}

// note that the game will crash if someone inserts a type that the game doesn't expect
void addGameSetting(cGameSettings settings, char* type, void* value) {
  GameSettings* s = (GameSettings*)settings;
  
  s->addSetting(type, value);
}

cGame initGame(cGameSettings settings, int simulatorID, double startTime, int* playerInfo, int playerLength, int seed) {
	GameSettings* s = (GameSettings*)settings;

	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = std::chrono::duration_cast<std::chrono::milliseconds>(now - seconds).count()/1000.0;
	double cnow = std::chrono::system_clock::to_time_t(now);
	double endTime = cnow + fraction;

	std::map<int, std::tuple<std::string, int, int>> players;

	for(int i = 0; i < playerLength; i++) {
		players[playerInfo[i * 3]] = std::make_tuple("Unnamed", playerInfo[i * 3 + 1], playerInfo[i * 3 + 2]);
	}

	Game* game = new Game(*s, simulatorID, startTime, endTime, players, seed, false);
	return (void*)game;
}

void freeGame(cGame game) {
  Game* g = (Game*)game;
  delete g;
}

void addOrder(cGame game, char* type, int ID, int referenceID, double timestamp, int senderID, int argumentIDs[], int argCount) {
  Game* g = (Game*)game;

  g->addOrder(std::string(type), ID, referenceID, timestamp, senderID, argumentIDs, argCount);
}

int runGame(cGame game, int* playerChanges) {
  Game* g = (Game*)game;

  std::list<std::pair<int, int>> changes = g->run();

  int i = 0;
  for(auto it = changes.begin(); it != changes.end(); it++, i++) {
    playerChanges[i * 2] = it->first;
    playerChanges[i * 2 + 1] = it->second;
  }

  return changes.empty() ? 0 : 1;
}
