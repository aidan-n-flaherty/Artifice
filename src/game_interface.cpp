#include "game_interface.h"
#include "floor_display.h"
#include "../GameLogic/gameClasses/game.h"
#include "../GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameLogic/gameClasses/gameObjects/specialist.h"
#include "../GameLogic/gameClasses/orders/send_order.h"
#include "../GameLogic/gameClasses/game_settings.h"
#include "../GameLogic/gameClasses/events/battle_event.h"
#include "../GameLogic/gameClasses/events/vessel_outpost_event.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <utility>
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
	
	ClassDB::bind_method(D_METHOD("init", "gameID", "userID", "startTime", "players", "settingOverrides"), &GameInterface::init);
	ClassDB::bind_method(D_METHOD("setTime", "t"), &GameInterface::setTime);
	ClassDB::bind_method(D_METHOD("getTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("setPercent", "percent"), &GameInterface::setPercent);
	ClassDB::bind_method(D_METHOD("getPercent"), &GameInterface::getPercent);
	ClassDB::bind_method(D_METHOD("getWidth"), &GameInterface::getWidth);
	ClassDB::bind_method(D_METHOD("getHeight"), &GameInterface::getHeight);
	ClassDB::bind_method(D_METHOD("getSimulationSpeed"), &GameInterface::getSimulationSpeed);
	ClassDB::bind_method(D_METHOD("getHires"), &GameInterface::getHires);
	ClassDB::bind_method(D_METHOD("getStartTime"), &GameInterface::getStartTime);
	ClassDB::bind_method(D_METHOD("canHire"), &GameInterface::canHire);
	ClassDB::bind_method(D_METHOD("hasStarted"), &GameInterface::hasStarted);
	ClassDB::bind_method(D_METHOD("hasEnded"), &GameInterface::hasEnded);
	ClassDB::bind_method(D_METHOD("getUserGameID"), &GameInterface::getUserGameID);
	ClassDB::bind_method(D_METHOD("getReferenceID"), &GameInterface::getReferenceID);
	ClassDB::bind_method(D_METHOD("getNextArrivalEvent"), &GameInterface::getNextArrivalEvent);
	ClassDB::bind_method(D_METHOD("getNextProductionEvent"), &GameInterface::getNextProductionEvent);
	ClassDB::bind_method(D_METHOD("getNextBattleEvent"), &GameInterface::getNextBattleEvent);
	ClassDB::bind_method(D_METHOD("getBattlePhases"), &GameInterface::getBattlePhases);
	ClassDB::bind_method(D_METHOD("getNextBattleMessages", "objID", "phase"), &GameInterface::getNextBattleMessages);
	ClassDB::bind_method(D_METHOD("getNextBattleStartingUnits", "objID"), &GameInterface::getNextBattleStartingUnits);
	ClassDB::bind_method(D_METHOD("getNextBattleUnits", "objID", "phase"), &GameInterface::getNextBattleUnits);
	ClassDB::bind_method(D_METHOD("getOutpostPositions"), &GameInterface::getOutpostPositions);
	ClassDB::bind_method(D_METHOD("getShopOptions"), &GameInterface::getShopOptions);
	ClassDB::bind_method(D_METHOD("getPromotionOptions"), &GameInterface::getPromotionOptions);
	ClassDB::bind_method(D_METHOD("getPlayerIDs"), &GameInterface::getPlayerIDs);
	ClassDB::bind_method(D_METHOD("getSortedPlayers"), &GameInterface::getSortedPlayers);
	ClassDB::bind_method(D_METHOD("getScore", "userID"), &GameInterface::getScore);
	ClassDB::bind_method(D_METHOD("getColor", "userID"), &GameInterface::getColor);
	ClassDB::bind_method(D_METHOD("getSpecialistName"), &GameInterface::getSpecialistName);
	ClassDB::bind_method(D_METHOD("getSpecialistDescription"), &GameInterface::getSpecialistDescription);
	ClassDB::bind_method(D_METHOD("getSpecialistType"), &GameInterface::getSpecialistType);
	ClassDB::bind_method(D_METHOD("getFloorDisplay"), &GameInterface::getFloorDisplay);
	ClassDB::bind_method(D_METHOD("shiftToTime", "t"), &GameInterface::shiftToTime);
	ClassDB::bind_method(D_METHOD("bulkAddOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::bulkAddOrder);
	ClassDB::bind_method(D_METHOD("endBulkAdd"), &GameInterface::endBulkAdd);
	ClassDB::bind_method(D_METHOD("addOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::addOrder);
	ClassDB::bind_method(D_METHOD("cancelOrder", "ID"), &GameInterface::cancelOrder);
	ADD_SIGNAL(MethodInfo("addOrder", PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::INT, "referenceID"), PropertyInfo(Variant::INT, "timestamp"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments")));
	ADD_SIGNAL(MethodInfo("selectVessel", PropertyInfo(Variant::OBJECT, "vessel")));
	ADD_SIGNAL(MethodInfo("selectOutpost", PropertyInfo(Variant::OBJECT, "outpost")));
	ADD_SIGNAL(MethodInfo("selectSpecialist", PropertyInfo(Variant::INT, "specialist")));
	ADD_SIGNAL(MethodInfo("deselect"));
	ADD_SIGNAL(MethodInfo("deselectSpecialist", PropertyInfo(Variant::INT, "specialist")));
	ADD_SIGNAL(MethodInfo("moveTo", PropertyInfo(Variant::FLOAT, "t")));
}

void GameInterface::init(int gameID, int userID, int startTime, Dictionary players, Dictionary settingOverrides) {
	this->gameID = gameID;
	this->userID = userID;

	this->settingOverrides = settingOverrides;

	current = getTimeMillis();
	
	std::map<int, std::tuple<std::string, int, int>> playerMap;
	playerMap[0] = std::make_tuple("Bob", 123456, 467);
	playerMap[1] = std::make_tuple("Joe", 654321, 752);
	playerMap[2] = std::make_tuple("Steve", 987654, 567);

	Array ids = players.keys();
	for(int i = 0; i < ids.size(); i++) {
		if(Variant::can_convert(ids[i].get_type(), Variant::INT)) {
			int id = int(ids[i]);

			Dictionary player = Dictionary(players[ids[i]]);

			int userID = int(player["id"]);
			int rating = int(Dictionary(player["userStats"])["rating"]);
			std::string name = std::string(String(player["username"]).utf8().get_data());

			playerMap[id] = std::make_tuple(name, userID, rating);
		}
	}
	
	settings = loadSettings();
	completeGame = std::shared_ptr<Game>(new Game(settings, userID, startTime, startTime + simulationBuffer / settings.simulationSpeed, playerMap, 42083, true));

	for(int i = 0; i < 30; i++) {
		std::list<int> specialists;
		completeGame->addOrder(new SendOrder(current + i * 20, 2, 1, specialists, 7, 5, completeGame->getReferenceID()));
	}
	for(int i = 0; i < 30; i++) {
		std::list<int> specialists;
		completeGame->addOrder(new SendOrder(current + 10 + i * 20, 2, 1, specialists, 7, 3, completeGame->getReferenceID()));
	}

	completeGame->run();
	nextEndState = completeGame->getNextEndState();

	userGameID = completeGame->getSimulatorID();

	current = getTimeMillis();
	update();
}

GameSettings GameInterface::loadSettings() {
	GameSettings settings;

	if(settingOverrides.has("simulationSpeed") && Variant::can_convert(settingOverrides["simulationSpeed"].get_type(), Variant::FLOAT)) {
		settings.simulationSpeed = double(settingOverrides["simulationSpeed"]);
	}

	return settings;
}

void GameInterface::_process(double delta) {
	Node::_process(delta);
	
	double time = getTimeMillis();
	if(future && current < time) current = time;

	update();
	
	if(game != nullptr) {
		double timeDiff = current - game->getTime();
		
		for(const auto& pair : vessels) pair.second->setDiff(time, timeDiff);
		
		for(const auto& pair : outposts) pair.second->setDiff(time, timeDiff);

		for(const auto& pair : players) pair.second->setDiff(time, timeDiff);

		floorDisplay->setDiff(timeDiff);
		floorDisplay->queue_redraw();
	}
}

void GameInterface::shiftToTime(double t) {
	emit_signal("moveTo", t);
}

void GameInterface::setTime(double t) {
	double time = getTimeMillis();
	
	future = t >= time;
	current = t;
}

void GameInterface::update() {
	if(game == nullptr || current + simulationBuffer / settings.simulationSpeed > nextEndState) {
		if(current > nextEndState) game = nullptr;

		completeGame = completeGame->lastState(nextEndState);
		completeGame->setEndTime(nextEndState);
		completeGame->run();
		nextEndState = completeGame->getNextEndState();
	}

	if(game == nullptr || current < game->getTime() || current >= nextState) {
		game = completeGame->lastState(current);
		nextState = completeGame->nextState(current);
		nextEndState = completeGame->getNextEndState();
		
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

		for(auto it = players.begin(); it != players.end();) {
			if(!game->hasPlayer(it->first)) {
				it->second->queue_free();
				remove_child(it->second);
				it = players.erase(it);
			} else it++;
		}


		for(auto it = selectedSpecialists.begin(); it != selectedSpecialists.end();) {
			if(!game->hasSpecialist(*it)) {
				setSelectedSpecialist(*it);
				it = selectedSpecialists.begin();
			} else it++;
		}

		if(selectedNode && !game->hasPosObject(selectedNode->getID())) unselect();
		
		for(const auto& pair : game->getVessels()) {
			if(vessels.find(pair.first) != vessels.end()) {
				vessels[pair.first]->setReference(pair.second);
			} else {
				VesselNode* newVessel = memnew(VesselNode(pair.second));
				newVessel->setReference(pair.second);
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
				newOutpost->setReference(pair.second);
				outposts[pair.first] = newOutpost;
				newOutpost->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				add_child(newOutpost);
			}
		}

		for(const auto& pair : game->getPlayers()) {
			if(players.find(pair.first) != players.end()) {
				players[pair.first]->setReference(pair.second);
			} else {
				PlayerNode* newPlayer = memnew(PlayerNode(pair.second));
				newPlayer->setReference(pair.second);
				players[pair.first] = newPlayer;
				add_child(newPlayer);
			}
		}
	}
}

PositionalNode* GameInterface::getObj(int id) {
	PositionalNode* obj = nullptr;

	if(vessels.find(id) != vessels.end()) obj = vessels[id];
	if(outposts.find(id) != outposts.end()) obj = outposts[id];

	return obj;
}

bool GameInterface::willSendWith(SpecialistType type) {
	if(!selectedNode) return false;

	for(Specialist* s : selectedNode->getObj()->getSpecialists()) {
		if(s->getType() == type && s->getOwnerID() == getUserGameID() && selectedSpecialists.find(s->getID()) != selectedSpecialists.end()) {
			return true;
		}
	}

	return false;
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

	PositionalNode* target = getObj(id);

	if(selectedNode) {
		VesselNode* v1 = dynamic_cast<VesselNode*>(selectedNode);
		OutpostNode* o1 = dynamic_cast<OutpostNode*>(selectedNode);

		VesselNode* v2 = dynamic_cast<VesselNode*>(target);
		OutpostNode* o2 = dynamic_cast<OutpostNode*>(target);


		if(o1) {
			if(v2 && !willSendWith(SpecialistType::PIRATE)) return;

			int units = selectedNode->getObj()->getUnitsAt(selectedNode->getDiff());

			if(units <= 0) {
				unselect();
				return;
			}


			int parameters[] = { std::max(1, int(percent * units)), selectedNode->getID(), target->getID() };
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

			while(!selectedSpecialists.empty()) {
				arguments.push_back(*selectedSpecialists.begin());
				selectedNode->setSpecialistSelected(*selectedSpecialists.begin(), false);
				selectedSpecialists.erase(selectedSpecialists.begin());
			}

			emit_signal("addOrder", "SEND", game->getReferenceID(), current, arguments);

			unselect();
			
			return;
		} else if(v1) {
			if(!selectedNode->getObj()->controlsSpecialist(SpecialistType::NAVIGATOR)) return;

			if(v2 && !selectedNode->getObj()->controlsSpecialist(SpecialistType::PIRATE)) return;

			int parameters[] = { selectedNode->getID(), target->getID() };
			Array arguments;

			for(int i = 0; i < 2; i++) arguments.push_back(parameters[i]);

			emit_signal("addOrder", "REROUTE", game->getReferenceID(), current, arguments);

			unselect();
			
			return;
		}
	} else if(target->getOwnerID() != getUserGameID()) return;

	setSelected(id);
}

void GameInterface::unselect() {
	setSelected(-1);

	emit_signal("deselect");

	while(!selectedSpecialists.empty()) setSelectedSpecialist(*selectedSpecialists.begin());
}

void GameInterface::setSelected(int id) {
	if(selectedNode) selectedNode->setSelected(false);

	PositionalNode* obj = getObj(id);

	if(obj) obj->setSelected(true);

	VesselNode* v = dynamic_cast<VesselNode*>(obj);
	OutpostNode* o = dynamic_cast<OutpostNode*>(obj);
	if(v) emit_signal("selectVessel", v);
	if(o) emit_signal("selectOutpost", o);

	selectedNode = obj;
}

void GameInterface::setSelectedSpecialist(int id) {
	bool selected = selectedSpecialists.find(id) == selectedSpecialists.end();

	Specialist* s = game->getSpecialist(id);

	int containerID = s->getContainer() ? s->getContainer()->getID() : -1;
	bool owned = s->getContainer() && s->getContainer()->getOwnerID() == getUserGameID();
	PositionalNode* container = getObj(containerID);

	for(auto& pair : vessels) if(pair.first != containerID || !owned) pair.second->clearSelectedSpecialists();
	for(auto& pair : outposts) if(pair.first != containerID || !owned) pair.second->clearSelectedSpecialists();

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
	
	if(selected) {
		if(owned) setSelected(containerID);
		selectedSpecialists.insert(id);
		emit_signal("selectSpecialist", id);
	} else {
		selectedSpecialists.erase(id);
		emit_signal("deselectSpecialist", id);
		if(owned) setSelected(containerID);
	}
}

void GameInterface::bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, timestamp, senderID, arr, argCount);
}

void GameInterface::endBulkAdd() {
	completeGame->run();
	game = nullptr;
	
	current += epsilon;
	
	update();
}

void GameInterface::addOrder(const String &type, uint32_t ID, int32_t referenceID, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, timestamp, senderID, arr, argCount);
	completeGame->run();
	game = nullptr;

	current += epsilon;
	
	update();
}

void GameInterface::cancelOrder(uint32_t ID) {
	completeGame = completeGame->removeOrder(ID);
	completeGame->run();
	game = nullptr;
	
	update();
}

PackedVector2Array GameInterface::getOutpostPositions() {
	PackedVector2Array arr;

	for(auto& pair : outposts) {
		arr.push_back(Vector2(pair.second->get_position().x, pair.second->get_position().z));
	}

	return arr;
}

PackedInt32Array GameInterface::getPlayerIDs() {
	PackedInt32Array arr;

	for(const auto& pair : game->getPlayers()) {
		arr.push_back(uint32_t(pair.second->getUserID()));
	}

	return arr;
}

Array GameInterface::getSortedPlayers() {
	Array arr;
	std::vector<Player*> sortedPlayers = game->sortedPlayers();

	for(Player* player_ : sortedPlayers) {
		arr.push_back(players[player_->getID()]);
	}

	return arr;
}

int GameInterface::getScore(int userID) {
	std::list<std::pair<int, int>> scores = game->getScores();

	for(const std::pair<int, int> &p : scores) {
		if(p.first == userID) return p.second;
	}

	return -1;
}

PackedInt32Array GameInterface::getShopOptions() {
	PackedInt32Array arr;

	for(SpecialistType t : Specialist::baseHires()) {
		arr.push_back(uint32_t(t));
	}

	return arr;
}

PackedInt32Array GameInterface::getPromotionOptions(int specialistID) {
	PackedInt32Array arr;

	for(SpecialistType t : game->getSpecialist(specialistID)->promotionOptions()) {
		arr.push_back(uint32_t(t));
	}

	return arr;
}

String GameInterface::getSpecialistName(int specialistNum) {
	return String(Specialist::typeAsString(SpecialistType(specialistNum)).c_str());
}

String GameInterface::getSpecialistDescription(int specialistNum) {
	return String(settings.specialistDescriptions[SpecialistType(specialistNum)].c_str());
}

double GameInterface::getNextArrivalEvent(int vesselID) {
	const VesselOutpostEvent* e = completeGame->nextArrival(vesselID, current);

	return e ? e->getTimestamp() : -1;
}

double GameInterface::getNextProductionEvent(int outpostID) {
	std::shared_ptr<Game> curr = game;

	double next = completeGame->nextState(curr->getTime());

	double time = curr->getTime();

	while(curr->getTime() + curr->getOutpost(outpostID)->nextProductionEvent() > next || curr->getTime() + curr->getOutpost(outpostID)->nextProductionEvent() <= current) {
		curr = completeGame->lastState(next);
		next = completeGame->nextState(curr->getTime());
		time = curr->getTime();
	}

	return time + curr->getOutpost(outpostID)->nextProductionEvent() + epsilon;
}

double GameInterface::getNextBattleEvent(int objID) {
	const BattleEvent* e = completeGame->nextBattle(objID, current);

	return e ? e->getTimestamp() : -1;
}

Array GameInterface::getBattlePhases() {
	Array arr;

	for(const std::string& str : BattleEvent::getPhases()) {
		arr.push_back(String(str.c_str()));
	}

	return arr;
}

Array GameInterface::getNextBattleUsers(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, current);

	Array arr;

	if(!b) return arr;

	arr.push_back(players[b->getBattleUsers().first]);
	arr.push_back(players[b->getBattleUsers().second]);

	return arr;
}

Array GameInterface::getNextBattleMessages(int objID, const String& phase) {
	const BattleEvent* b = completeGame->nextBattle(objID, current);

	Array arr;

	if(!b) return arr;

	for(const std::pair<int, std::string> &p : b->getBattleLog(std::string(phase.utf8().get_data()))) {
		Array pair;

		pair.push_back(p.first);
		pair.push_back(String(p.second.c_str()));

		arr.push_back(pair);
	}

	return arr;
}

Dictionary GameInterface::getNextBattleStartingUnits(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, current);

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getStartingUnits();

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}

Dictionary GameInterface::getNextBattleUnits(int objID, const String& phase) {
	const BattleEvent* b = completeGame->nextBattle(objID, current);

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getPhaseUnits(std::string(phase.utf8().get_data()));

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}

Color GameInterface::getColor(int userID) {
	for(auto pair : game->getPlayers()) {
		if(pair.second->getUserID() == userID) {
			std::tuple<double, double, double> color = settings.playerColors[pair.first % settings.playerColors.size()];

			return Color(std::get<0>(color), std::get<1>(color), std::get<2>(color));
		}
	}

	return Color(0.0, 0.0, 0.0);
}

