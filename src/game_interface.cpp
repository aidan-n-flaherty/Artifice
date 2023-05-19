#include "game_interface.h"
#include "../GameServer/GameLogic/gameClasses/game.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/specialist.h"
#include "../GameServer/GameLogic/gameClasses/orders/send_order.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <map>
#include <list>

using namespace godot;

void GameInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("select"), &GameInterface::select);
	
	ClassDB::bind_method(D_METHOD("setTime", "t"), &GameInterface::setTime);
	ClassDB::bind_method(D_METHOD("getTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("addOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::addOrder);
	ADD_SIGNAL(MethodInfo("addOrder", PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::INT, "referenceID"), PropertyInfo(Variant::INT, "timestamp"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments")));
}

GameInterface::GameInterface() {
	init();
}

void GameInterface::init() {
	time(&current);
	
	std::map<int, std::string> players;
	players[123456] = "Bob";
	players[654321] = "Joe";
	players[987654] = "Steve";
	
	completeGame = std::shared_ptr<Game>(new Game(654321, current, current + 48 * 60 * 60, players, 42083, true));
	/*std::list<int> specialists;
	specialists.push_back(4);
	completeGame->addOrder(new SendOrder(current, 0, 20, specialists, 3, 7, completeGame->getReferenceID()));
	specialists = std::list<int>();
	specialists.push_back(8);
	completeGame->addOrder(new SendOrder(current, 2, 20, specialists, 7, 3, completeGame->getReferenceID()));
	specialists = std::list<int>();
	specialists.push_back(6);
	completeGame->addOrder(new SendOrder(current + 1, 1, 10, specialists, 5, 12, 0));*/
	completeGame->run();
	
	setTime(current);
}

void GameInterface::_process(double delta) {
	Node::_process(delta);
	
	time_t tmp;
	time(&tmp);
	if(future && current < tmp) setTime(tmp);
	
	update();
}

void GameInterface::setTime(uint32_t t) {
	time_t tmp;
	time(&tmp);
	
	if(t >= tmp) future = true;
	current = t;
}

void GameInterface::update() {
	if(game == nullptr || current < game->getTime() || current >= nextState) {
		game = completeGame->lastState(current);
		nextState = completeGame->nextState(current);
		
		for(auto it = vessels.begin(); it != vessels.end();) {
			if(!game->hasVessel(it->first)) {
				remove_child(it->second);
				it = vessels.erase(it);
			} else it++;
		}
		
		for(auto it = outposts.begin(); it != outposts.end();) {
			if(!game->hasOutpost(it->first)) {
				remove_child(it->second);
				it = outposts.erase(it);
			} else it++;
		}
		
		for(const auto& pair : game->getVessels()) {
			if(vessels.count(pair.first) > 0) {
				vessels[pair.first]->setReference(pair.second, game->getTime());
			} else {
				VesselNode* newVessel = memnew(VesselNode(pair.second, game->getTime()));
				vessels[pair.first] = newVessel;
				newVessel->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				add_child(newVessel);
			}
		}
		
		for(const auto& pair : game->getOutposts()) {
			if(outposts.count(pair.first) > 0) {
				outposts[pair.first]->setReference(pair.second, game->getTime());
			} else {
				OutpostNode* newOutpost = memnew(OutpostNode(pair.second, game->getTime()));
				outposts[pair.first] = newOutpost;
				newOutpost->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				add_child(newOutpost);
			}
		}
	}
}

void GameInterface::select(int id) {
	for(const auto& pair1 : outposts) {
		if(pair1.first == selectedObj) {
			for(const auto& pair2 : outposts) {
				if(pair2.first == id) {
					int parameters[] = { 1, pair1.first, pair2.first };
					Array arguments;
					for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);
					emit_signal("addOrder", "SEND", game->getReferenceID(), int(current), arguments);
					id = -1;
					selectedObj = -1;
					break;
				}
			}
		}
	}
	
	selectedObj = id;
}

void GameInterface::bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];
	std::string t;
	
	if(type == "SEND") t = "SEND";
	else if(type == "HIRE") t = "HIRE";
	else if(type == "GIFT") t = "GIFT";
	else if(type == "PROMOTE") t = "PROMOTE";
	else if(type == "REROUTE") t = "REROUTE";

	completeGame = completeGame->processOrder(t, ID, referenceID, timestamp, senderID, arr, argCount);
}

void GameInterface::endBulkAdd() {
	completeGame->run();
	game = nullptr;
	
	update();
}

void GameInterface::addOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];
	std::string t;
	
	if(type == "SEND") t = "SEND";
	else if(type == "HIRE") t = "HIRE";
	else if(type == "GIFT") t = "GIFT";
	else if(type == "PROMOTE") t = "PROMOTE";
	else if(type == "REROUTE") t = "REROUTE";

	completeGame = completeGame->processOrder(t, ID, referenceID, timestamp, senderID, arr, argCount);
	completeGame->run();
	game = nullptr;
	
	update();
}