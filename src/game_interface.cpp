#include "game_interface.h"
#include "floor_display.h"
#include "../GameServer/GameLogic/gameClasses/game.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/specialist.h"
#include "../GameServer/GameLogic/gameClasses/orders/send_order.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <map>
#include <list>

using namespace godot;

double getTimeMillis() {
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = std::chrono::duration_cast<std::chrono::milliseconds>(now - seconds).count()/1000.0;
	double cnow = std::chrono::system_clock::to_time_t(now);

	return cnow + fraction;
}

void GameInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("select"), &GameInterface::select);
	
	ClassDB::bind_method(D_METHOD("init", "userID"), &GameInterface::init);
	ClassDB::bind_method(D_METHOD("setTime", "t"), &GameInterface::setTime);
	ClassDB::bind_method(D_METHOD("getTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("setPercent", "percent"), &GameInterface::setPercent);
	ClassDB::bind_method(D_METHOD("getPercent"), &GameInterface::getPercent);
	ClassDB::bind_method(D_METHOD("getWidth"), &GameInterface::getWidth);
	ClassDB::bind_method(D_METHOD("getHeight"), &GameInterface::getHeight);
	ClassDB::bind_method(D_METHOD("getFloorDisplay"), &GameInterface::getFloorDisplay);
	ClassDB::bind_method(D_METHOD("addOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::addOrder);
	ADD_SIGNAL(MethodInfo("addOrder", PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::INT, "referenceID"), PropertyInfo(Variant::INT, "timestamp"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments")));
}

void GameInterface::init(int userID) {
	this->userID = userID;

	current = getTimeMillis();
	
	std::map<int, std::string> players;
	players[123456] = "Bob";
	players[654321] = "Joe";
	players[987654] = "Steve";
	
	completeGame = std::shared_ptr<Game>(new Game(userID, current, current + 48 * 60 * 60, players, 42083, true));
	/*std::list<int> specialists;
	specialists.push_back(4);
	completeGame->addOrder(new SendOrder(current, 0, 20, specialists, 3, 7, completeGame->getReferenceID()));
	specialists = std::list<int>();
	specialists.push_back(8);
	completeGame->addOrder(new SendOrder(current, 2, 20, specialists, 7, 3, completeGame->getReferenceID()));
	specialists = std::list<int>();
	specialists.push_back(6);
	completeGame->addOrder(new SendOrder(current + 1, 1, 10, specialists, 5, 12, 0));*/
	for(int i = 0; i < 10; i++) {
		std::list<int> specialists;
		completeGame->addOrder(new SendOrder(current + i * 20, 2, 1, specialists, 7, 5, completeGame->getReferenceID()));
	}
	for(int i = 0; i < 10; i++) {
		std::list<int> specialists;
		completeGame->addOrder(new SendOrder(current + 10 + i * 20, 2, 1, specialists, 7, 3, completeGame->getReferenceID()));
	}
	completeGame->run();

	userGameID = completeGame->getSimulatorID();
}

void GameInterface::_process(double delta) {
	Node::_process(delta);
	
	double time = getTimeMillis();
	if(future && current < time) current = time;
	
	update();
	
	if(game != nullptr) {
		double timeDiff = current - game->getTime();
		
		for(const auto& pair : vessels) pair.second->setDiff(timeDiff);
		
		for(const auto& pair : outposts) pair.second->setDiff(timeDiff);

		floorDisplay->setDiff(timeDiff);
		floorDisplay->queue_redraw();
	}
}

void GameInterface::setTime(double t) {
	double time = getTimeMillis();
	
	future = t >= time;
	current = t;
}

void GameInterface::update() {
	if(game == nullptr || current < game->getTime() || current >= nextState) {
		game = completeGame->lastState(current);
		nextState = completeGame->nextState(current);
		
		for(auto it = vessels.begin(); it != vessels.end();) {
			if(!game->hasVessel(it->first)) {
				it->second->queue_free();
				remove_child(it->second);
				it = vessels.erase(it);
			} else it++;
		}
		
		for(auto it = outposts.begin(); it != outposts.end();) {
			if(!game->hasOutpost(it->first)) {
				it->second->queue_free();
				remove_child(it->second);
				it = outposts.erase(it);
			} else it++;
		}

		if(selectedNode && !game->hasPosObject(selectedNode->getID())) unselect();

		for(auto it = selectedSpecialists.begin(); it != selectedSpecialists.end();) {
			if(game->hasSpecialist(*it) && game->getSpecialist(*it)->getContainer() && game->hasPosObject(game->getSpecialist(*it)->getContainer()->getID())) {
				PositionalObject* container = game->getSpecialist(*it)->getContainer();
				std::list<Specialist*> specialists = container->getSpecialists();

				if(!std::any_of(specialists.begin(), specialists.end(), [&it](auto& s){
					return s->getID() == *it;
				})) {
					if(vessels.find(container->getID()) != vessels.end()) vessels[container->getID()]->setSpecialistSelected(*it, false);
					if(outposts.find(container->getID()) != outposts.end()) outposts[container->getID()]->setSpecialistSelected(*it, false);

					it = selectedSpecialists.erase(it);
				} else it++;
			} else it = selectedSpecialists.erase(it);
		}
		
		for(const auto& pair : game->getVessels()) {
			if(vessels.find(pair.first) != vessels.end()) {
				vessels[pair.first]->setReference(pair.second);
			} else {
				VesselNode* newVessel = memnew(VesselNode(pair.second));
				vessels[pair.first] = newVessel;
				newVessel->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				add_child(newVessel);
			}
		}
		
		for(const auto& pair : game->getOutposts()) {
			if(outposts.find(pair.first) != outposts.end()) {
				outposts[pair.first]->setReference(pair.second);
			} else {
				OutpostNode* newOutpost = memnew(OutpostNode(pair.second));
				outposts[pair.first] = newOutpost;
				newOutpost->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				add_child(newOutpost);
			}
		}
	}
}

// event propagated from positional nodes, occurs when something is clicked on
void GameInterface::select(int id) {
	// selecting the same thing deselects
	if(selectedNode && id == selectedNode->getID()) {
		unselect();
		
		return;
	}

	if(game->hasSpecialist(id)) {
		setSelectedSpecialist(id);
		return;
	}

	// if an outpost is already selected, check if another outpost is being selected to send a vessel to
	if(outposts.find(id) != outposts.end()) {
		OutpostNode* outpost = outposts[id];

		if(dynamic_cast<OutpostNode*>(selectedNode)) {
			int units = selectedNode->getObj()->getUnitsAt(selectedNode->getDiff());

			if(units <= 0) {
				unselect();
				return;
			}

			int parameters[] = { std::max(1, int(percent * units)), selectedNode->getID(), outpost->getID() };
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

			while(!selectedSpecialists.empty()) {
				arguments.push_back(*selectedSpecialists.begin());
				selectedSpecialists.erase(selectedSpecialists.begin());
			}

			emit_signal("addOrder", "SEND", game->getReferenceID(), int(current), arguments);

			unselect();
			
			return;
		} else if(outpost->getOwnerID() != getUserGameID()) return;
	} else if(vessels.find(id) != vessels.end()) {
		VesselNode* vessel = vessels[id];

		if(dynamic_cast<OutpostNode*>(selectedNode)) {
			int units = selectedNode->getObj()->getUnitsAt(selectedNode->getDiff());

			if(units <= 0) {
				unselect();
				return;
			}

			bool hasPirate = false;

			for(Specialist* s : selectedNode->getObj()->getSpecialists()) {
				if(s->getType() == SpecialistType::PIRATE && selectedSpecialists.find(s->getID()) != selectedSpecialists.end()) {
					hasPirate = true;
					break;
				}
			}

			if(!hasPirate) return;

			int parameters[] = { std::max(1, int(percent * units)), selectedNode->getID(), vessel->getID() };
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

			while(!selectedSpecialists.empty()) {
				arguments.push_back(*selectedSpecialists.begin());
				selectedNode->setSpecialistSelected(*selectedSpecialists.begin(), false);
				selectedSpecialists.erase(selectedSpecialists.begin());
			}

			emit_signal("addOrder", "SEND", game->getReferenceID(), int(current), arguments);

			unselect();
			
			return;
		} else if(vessel->getOwnerID() != getUserGameID()) return;
	}
	
	setSelected(id);
}

void GameInterface::unselect() {
	setSelected(-1);

	while(!selectedSpecialists.empty()) setSelectedSpecialist(*selectedSpecialists.begin());
}

void GameInterface::setSelected(int id) {
	int selectedObj = selectedNode ? selectedNode->getID() : -1;

	if(vessels.find(selectedObj) != vessels.end()) vessels[selectedObj]->setSelected(false);
	if(outposts.find(selectedObj) != outposts.end()) outposts[selectedObj]->setSelected(false);

	selectedObj = id;

	PositionalNode* obj = nullptr;

	if(vessels.find(selectedObj) != vessels.end()) (obj = vessels[selectedObj])->setSelected(true);
	if(outposts.find(selectedObj) != outposts.end()) (obj = outposts[selectedObj])->setSelected(true);

	selectedNode = obj;
}

void GameInterface::setSelectedSpecialist(int id) {
	bool selected = selectedSpecialists.find(id) == selectedSpecialists.end();

	Specialist* s = game->getSpecialist(id);
	PositionalNode* container;

	int containerID = s->getContainer() ? s->getContainer()->getID() : -1;
	bool owned = s->getContainer() && s->getContainer()->getOwnerID() == getUserGameID();

	if(vessels.find(containerID) != vessels.end()) {
		container = vessels[containerID];
		for(auto& pair : vessels) if(pair.first != containerID || !owned) pair.second->clearSelectedSpecialists();
	}

	if(outposts.find(containerID) != outposts.end()) {
		container = outposts[containerID];
		for(auto& pair : outposts) if(pair.first != containerID || !owned) pair.second->clearSelectedSpecialists();
	}

	if(container) {
		container->setSpecialistSelected(id, selected);

		std::list<Specialist*> specialists = container->getObj()->getSpecialists();
		
		for(auto it = selectedSpecialists.begin(); it != selectedSpecialists.end();) {
			if(!std::any_of(specialists.begin(), specialists.end(), [&it](auto s){
				return *it == s->getID();
			})) it = selectedSpecialists.erase(it);
			else it++;
		}
	}
	if(owned) setSelected(containerID);
	
	if(selected) selectedSpecialists.insert(id);
	else selectedSpecialists.erase(id);
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