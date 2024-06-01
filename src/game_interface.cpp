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
#include "../GameLogic/gameClasses/events/win_condition_event.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <utility>
#include <map>
#include <list>

using namespace godot;

double GameInterface::getTimeMillis() {
	auto now = std::chrono::system_clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = std::chrono::duration_cast<std::chrono::milliseconds>(now - seconds).count()/1000.0;
	double cnow = std::chrono::system_clock::to_time_t(now);

	return cnow + fraction;
}

void GameInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("suspend"), &GameInterface::suspend);
	ClassDB::bind_method(D_METHOD("resume"), &GameInterface::resume);
	ClassDB::bind_method(D_METHOD("select", "id"), &GameInterface::select);
	ClassDB::bind_method(D_METHOD("sendTo", "id"), &GameInterface::sendTo);
	ClassDB::bind_method(D_METHOD("release", "id"), &GameInterface::release);
	ClassDB::bind_method(D_METHOD("unselect"), &GameInterface::unselect);
	ClassDB::bind_method(D_METHOD("canStartDrag"), &GameInterface::canStartDrag);
	ClassDB::bind_method(D_METHOD("justSelected"), &GameInterface::justSelected);
	ClassDB::bind_method(D_METHOD("getSelectedUnits"), &GameInterface::getSelectedUnits);
	ClassDB::bind_method(D_METHOD("ownsObj"), &GameInterface::ownsObj);
	ClassDB::bind_method(D_METHOD("getTarget", "x", "y"), &GameInterface::getTarget);
	ClassDB::bind_method(D_METHOD("projectedTime", "x", "y"), &GameInterface::projectedTime);
	ClassDB::bind_method(D_METHOD("setMouse", "x", "y"), &GameInterface::setMouse);
	ClassDB::bind_method(D_METHOD("setDrag", "drag"), &GameInterface::setDrag);
	ClassDB::bind_method(D_METHOD("getWinCondition"), &GameInterface::getWinCondition);
	ClassDB::bind_method(D_METHOD("init", "gameID", "userID", "seed", "startTime", "playerCap", "players", "settingOverrides"), &GameInterface::init);
	ClassDB::bind_method(D_METHOD("startAtEnd"), &GameInterface::startAtEnd);
	ClassDB::bind_method(D_METHOD("setTempTime", "t"), &GameInterface::setTempTime);
	ClassDB::bind_method(D_METHOD("setTime", "t"), &GameInterface::setTime);
	ClassDB::bind_method(D_METHOD("getTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("getBuffTime"), &GameInterface::getBuffTime);
	ClassDB::bind_method(D_METHOD("setBuff"), &GameInterface::setBuff);
	ClassDB::bind_method(D_METHOD("getBuff"), &GameInterface::getBuff);
	ClassDB::bind_method(D_METHOD("getClientTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("clientToGameTime", "t"), &GameInterface::clientToGameTime);
	ClassDB::bind_method(D_METHOD("gameToClientTime", "t"), &GameInterface::gameToClientTime);
	ClassDB::bind_method(D_METHOD("setPercent", "percent"), &GameInterface::setPercent);
	ClassDB::bind_method(D_METHOD("getPercent"), &GameInterface::getPercent);
	ClassDB::bind_method(D_METHOD("getWidth"), &GameInterface::getWidth);
	ClassDB::bind_method(D_METHOD("getHeight"), &GameInterface::getHeight);
	ClassDB::bind_method(D_METHOD("getSimulationSpeed"), &GameInterface::getSimulationSpeed);
	ClassDB::bind_method(D_METHOD("getNextHireEvent"), &GameInterface::getNextHireEvent);
	ClassDB::bind_method(D_METHOD("getHires"), &GameInterface::getHires);
	ClassDB::bind_method(D_METHOD("getStartTime"), &GameInterface::getStartTime);
	ClassDB::bind_method(D_METHOD("hasLost"), &GameInterface::hasLost);
	ClassDB::bind_method(D_METHOD("canHire"), &GameInterface::canHire);
	ClassDB::bind_method(D_METHOD("canRelease", "specialistID"), &GameInterface::canRelease);
	ClassDB::bind_method(D_METHOD("canPromote", "specialistID"), &GameInterface::canPromote);
	ClassDB::bind_method(D_METHOD("canUndoSpecialist", "specialistID"), &GameInterface::canUndoSpecialist);
	ClassDB::bind_method(D_METHOD("getSpecialistOriginatingOrder", "specialistID"), &GameInterface::getSpecialistOriginatingOrder);
	ClassDB::bind_method(D_METHOD("getSpecialistOriginatingOrderType", "specialistID"), &GameInterface::getSpecialistOriginatingOrderType);
	ClassDB::bind_method(D_METHOD("isMining"), &GameInterface::isMining);
	ClassDB::bind_method(D_METHOD("isConquest"), &GameInterface::isConquest);
	ClassDB::bind_method(D_METHOD("isElimination"), &GameInterface::isElimination);
	ClassDB::bind_method(D_METHOD("isPaused"), &GameInterface::isPaused);
	ClassDB::bind_method(D_METHOD("ownsSpecialist", "specialistID"), &GameInterface::ownsSpecialist);
	ClassDB::bind_method(D_METHOD("getSpecialistOwner", "specialistID"), &GameInterface::getSpecialistOwner);
	ClassDB::bind_method(D_METHOD("hasStarted"), &GameInterface::hasStarted);
	ClassDB::bind_method(D_METHOD("hasEnded"), &GameInterface::hasEnded);
	ClassDB::bind_method(D_METHOD("getUserGameID"), &GameInterface::getUserGameID);
	ClassDB::bind_method(D_METHOD("getReferenceID"), &GameInterface::getReferenceID);
	ClassDB::bind_method(D_METHOD("getNextVictoryMessage"), &GameInterface::getNextVictoryMessage);
	ClassDB::bind_method(D_METHOD("getNextVictoryTime"), &GameInterface::getNextVictoryTime);
	ClassDB::bind_method(D_METHOD("getNextVictoryPlayer"), &GameInterface::getNextVictoryPlayer);
	ClassDB::bind_method(D_METHOD("getNextArrivalEvent"), &GameInterface::getNextArrivalEvent);
	ClassDB::bind_method(D_METHOD("getNextProductionEvent"), &GameInterface::getNextProductionEvent);
	ClassDB::bind_method(D_METHOD("getNextBattleEvent"), &GameInterface::getNextBattleEvent);
	ClassDB::bind_method(D_METHOD("canViewNextBattle", "objID"), &GameInterface::canViewNextBattle);
	ClassDB::bind_method(D_METHOD("getBattlePhases"), &GameInterface::getBattlePhases);
	ClassDB::bind_method(D_METHOD("getNextBattleMessages", "objID", "phase"), &GameInterface::getNextBattleMessages);
	ClassDB::bind_method(D_METHOD("getNextBattleStartingUnits", "objID"), &GameInterface::getNextBattleStartingUnits);
	ClassDB::bind_method(D_METHOD("getNextBattleUnits", "obfjID", "phase"), &GameInterface::getNextBattleUnits);
	ClassDB::bind_method(D_METHOD("getNextBattleUsers", "objID"), &GameInterface::getNextBattleUsers);
	ClassDB::bind_method(D_METHOD("getNextBattlePreVictoryUnits", "objID"), &GameInterface::getNextBattlePreVictoryUnits);
	ClassDB::bind_method(D_METHOD("getNextBattleShields", "objID"), &GameInterface::getNextBattleShields);
	ClassDB::bind_method(D_METHOD("getNextBattleVictor", "objID"), &GameInterface::getNextBattleVictor);
	ClassDB::bind_method(D_METHOD("getNextBattleVictorUnits", "objID"), &GameInterface::getNextBattleVictorUnits);
	ClassDB::bind_method(D_METHOD("getNextBattleCaptures", "objID"), &GameInterface::getNextBattleCaptures);
	ClassDB::bind_method(D_METHOD("getOutpostPositions"), &GameInterface::getOutpostPositions);
	ClassDB::bind_method(D_METHOD("getAllSpecialists"), &GameInterface::getAllSpecialists);
	ClassDB::bind_method(D_METHOD("getShopOptions"), &GameInterface::getShopOptions);
	ClassDB::bind_method(D_METHOD("getPromotionOptions"), &GameInterface::getPromotionOptions);
	ClassDB::bind_method(D_METHOD("getPlayerIDs"), &GameInterface::getPlayerIDs);
	ClassDB::bind_method(D_METHOD("getPlayers"), &GameInterface::getPlayers);
	ClassDB::bind_method(D_METHOD("getPlayer"), &GameInterface::getPlayer);
	ClassDB::bind_method(D_METHOD("getSortedPlayers"), &GameInterface::getSortedPlayers);
	ClassDB::bind_method(D_METHOD("getCurrentSortedPlayers"), &GameInterface::getCurrentSortedPlayers);
	ClassDB::bind_method(D_METHOD("getScore", "userID"), &GameInterface::getScore);
	ClassDB::bind_method(D_METHOD("getColor", "userID"), &GameInterface::getColor);
	ClassDB::bind_method(D_METHOD("getSpecialistName"), &GameInterface::getSpecialistName);
	ClassDB::bind_method(D_METHOD("getSpecialistDescription"), &GameInterface::getSpecialistDescription);
	ClassDB::bind_method(D_METHOD("getSpecialistHireAmount"), &GameInterface::getSpecialistHireAmount);
	ClassDB::bind_method(D_METHOD("getSpecialistType"), &GameInterface::getSpecialistType);
	ClassDB::bind_method(D_METHOD("getNumTeams"), &GameInterface::getNumTeams);
	ClassDB::bind_method(D_METHOD("getFloorDisplay"), &GameInterface::getFloorDisplay);
	ClassDB::bind_method(D_METHOD("getNode", "id"), &GameInterface::getNode);
	ClassDB::bind_method(D_METHOD("getSpawnLocation"), &GameInterface::getSpawnLocation);
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

void GameInterface::suspend() {
	this->game = nullptr;
	this->simulatedGame = nullptr;

	for(auto it = vessels.begin(); it != vessels.end();) {
		it->second->queue_free();
		remove_child(it->second);
		it = vessels.erase(it);
	}

	for(auto it = outposts.begin(); it != outposts.end();) {
		it->second->queue_free();
		remove_child(it->second);
		it = outposts.erase(it);
	}

	paused = true;
}


void GameInterface::resume() {
	paused = false;

	update();
}

void GameInterface::init(int gameID, int userID, int seed, int startTime, bool finished, int playerCap, Dictionary players, Dictionary settingOverrides) {
	// RESET MEMBER VARIABLES
	this->completeGame = nullptr;
	this->game = nullptr;
	this->currentGame = nullptr;
	this->simulatedGame = nullptr;

	for(auto it = this->vessels.begin(); it != this->vessels.end();) {
		it->second->queue_free();
		remove_child(it->second);
		it = this->vessels.erase(it);
	}

	for(auto it = this->outposts.begin(); it != this->outposts.end();) {
		it->second->queue_free();
		remove_child(it->second);
		it = this->outposts.erase(it);
	}

	for(auto it = this->players.begin(); it != this->players.end();) {
		it->second->queue_free();
		remove_child(it->second);
		it = this->players.erase(it);
	}
	
	this->vessels.clear();
	this->outposts.clear();
	this->players.clear();

	this->selected = -1;
	this->selectedSpecialists.clear();
	
	// END RESET

	this->gameID = gameID;
	this->userID = userID;
	this->finished = finished;

	this->settingOverrides = settingOverrides;

	current = getTimeMillis();
	
	std::map<int, std::tuple<std::string, int, int>> playerMap;
	for(int i = 0; i < playerCap; i++) {
		playerMap[i] = std::make_tuple("Unclaimed", -1, 0);
	}

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
	settings.startTime = startTime;
	completeGame = std::shared_ptr<Game>(new Game(settings, userID, startTime, settings.clientToGameTime(startTime + simulationBuffer / settings.simulationSpeed), playerMap, seed, true));

	settings = *(completeGame->getSettings());

	completeGame->run();
	nextEndState = completeGame->getNextEndState();

	userGameID = completeGame->getSimulatorID();

	current = getTimeMillis();
	update();
}

GameSettings GameInterface::loadSettings() {
	GameSettings settings;

	Array keys = settingOverrides.keys();

	for(int i = 0; i < keys.size(); i++) {
		bool allocated = false;

		const void* value;
		std::string str = "";

		if(Variant::can_convert(settingOverrides[keys[i]].get_type(), Variant::ARRAY)) {
			Array arr = Array(settingOverrides[keys[i]]);

			for(int j = 0; j < arr.size(); j++) {
				if(!Variant::can_convert(arr[j].get_type(), Variant::FLOAT)) continue;

				if(j > 0) str += " ";
				str += std::to_string(int(std::round(double(arr[j]))));
			}

			value = str.c_str();
		} else if(settingOverrides[keys[i]].get_type() == Variant::STRING) {
			value = calloc(64, sizeof(char));
			allocated = true;
			strncpy((char*)value, String(settingOverrides[keys[i]]).utf8().get_data(), 63);
		} else if(Variant::can_convert(settingOverrides[keys[i]].get_type(), Variant::FLOAT)) {
			value = new double(settingOverrides[keys[i]]);
			allocated = true;
		} else {
			value = nullptr;
		}

		if(Variant::can_convert(keys[i].get_type(), Variant::STRING)) settings.addSetting(String(keys[i]).utf8().get_data(), value);

		if(allocated) delete value;
	}

	std::cout << "Game mode " << settings.gameMode << std::endl;

	return settings;
}

void GameInterface::_process(double delta) {
	Node::_process(delta);
	
	double time = getTimeMillis();
	if(future) {
		if(buffer) {
			double buffTime = getBuffTime();
			if(current < buffTime) current = buffTime;
		} else if(current < time) current = time;
	}
	
	update();
	
	if(game != nullptr && simulatedGame != nullptr && currentGame != nullptr) {
		double t = settings.clientToGameTime(time);

		double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();

		double simulatedDiff = settings.clientToGameTime(getCurrent()) - simulatedGame->getTime();

		Player* p = future ? currentGame->getPlayer(getUserGameID()) : game->getPlayer(getUserGameID());
    
		std::shared_ptr<Game> visibilityGame = future ? currentGame : game;
		
		for(const auto& pair : vessels) {
			pair.second->setDiff(t, timeDiff);
			pair.second->setSelfOwned(pair.second->getOwnerID() == userGameID && userGameID >= 0);
			pair.second->setInRadar(finished || (p ? visibilityGame->withinRange(p, pair.second->getObj(), timeDiff) : false));
			pair.second->set_visible(finished || (p ? visibilityGame->withinRange(p, pair.second->getObj(), timeDiff) : false));
		}
		
		for(const auto& pair : outposts) {
			pair.second->setDiff(t, timeDiff);
			pair.second->setSelfOwned(pair.second->getOwnerID() == userGameID && userGameID >= 0);
			pair.second->setInRadar(finished || (p ? visibilityGame->withinRange(p, pair.second->getObj(), timeDiff) : false));
			pair.second->set_visible(true);
			pair.second->setViewType(finished || (p ? p->controlsSpecialist(SpecialistType::INTELLIGENCE_OFFICER) : false));
		}

		for(const auto& pair : players) pair.second->setDiff(t, timeDiff);

		if(selected >= 0 && getNode(selected) && getNode(selected)->isInRadar()) selectedUnits = getSelected()->getUnitsAt(timeDiff);
		else selectedUnits = -1;

		floorDisplay->setDiff(timeDiff, simulatedDiff);
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
	double time = settings.clientToGameTime(getTime());
	double currentTime = settings.clientToGameTime(getCurrent());
	double timeMillis = settings.clientToGameTime(getTimeMillis());
	
	if(game == nullptr || time + simulationBuffer / settings.simulationSpeed > nextEndState) {
		if(time > nextEndState) game = nullptr;

		completeGame = completeGame->lastState(nextEndState);
		completeGame->setEndTime(nextEndState);
		completeGame->run();
		nextEndState = completeGame->getNextEndState();
	}

	if(currentGame == nullptr || timeMillis >= nextCurrentState) {
		currentGame = completeGame->lastState(timeMillis);
		nextCurrentState = completeGame->nextState(timeMillis);
	}

	if(simulatedGame == nullptr || currentTime < simulatedGame->getTime() || currentTime >= nextSimulatedState) {
		simulatedGame = completeGame->lastState(currentTime);
		nextSimulatedState = completeGame->nextState(currentTime);

		if(selected >= 0 && !getSelected()) unselect();
	}

	if(game == nullptr || time < game->getTime() || time >= nextState) {
		game = completeGame->lastState(time);
		nextState = completeGame->nextState(time);
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
		
		for(const auto& pair : game->getVessels()) {
			if(vessels.find(pair.first) != vessels.end()) {
				vessels[pair.first]->setReference(pair.second);
			} else {
				VesselNode* newVessel = memnew(VesselNode(pair.second));
				newVessel->setReference(pair.second);
				vessels[pair.first] = newVessel;
				newVessel->connect("selected", Callable(this, "select"), Object::CONNECT_DEFERRED);
				newVessel->connect("released", Callable(this, "release"), Object::CONNECT_DEFERRED);
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
				newOutpost->connect("released", Callable(this, "release"), Object::CONNECT_DEFERRED);
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

		for(auto it = selectedSpecialists.begin(); it != selectedSpecialists.end();) {
			if(!game->hasSpecialist(*it)) {
				setSelectedSpecialist(*it);
				it = selectedSpecialists.begin();
			} else it++;
		}

		if(selected >= 0 && !getSelected()) unselect();
	}
}

PositionalNode* GameInterface::getNode(int id) {
	PositionalNode* obj = nullptr;

	if(vessels.find(id) != vessels.end()) obj = vessels[id];
	if(outposts.find(id) != outposts.end()) obj = outposts[id];

	return obj;
}

bool GameInterface::willSendWith(SpecialistType type) {
	if(!getSelected()) return false;
	
	Vessel* v = dynamic_cast<Vessel*>(getSelected());

	for(Specialist* s : getSelected()->getSpecialists()) {
		if(s->getType() == type && s->getOwnerID() == getUserGameID() && (v || selectedSpecialists.find(s->getID()) != selectedSpecialists.end())) {
			return true;
		}
	}

	return false;
}

void GameInterface::release(int id) {
	bool didDrag = dragged;
	dragged = false;
	startDrag = false;

	if(!didDrag) {
		bool selectedSpecialist = selectedSpecialists.find(id) != selectedSpecialists.end();

		if(game->hasSpecialist(id) && selectedSpecialist && !justSelectedSpecialist) {
			setSelectedSpecialist(id);
		}
	}

	justSelectedSpecialist = false;
}

void GameInterface::sendTo(int id) {
	PositionalObject* target = getObj(id);

	double timeDiff = settings.clientToGameTime(getCurrent()) - game->getTime();

	if(getSelected() && getSelected()->getOwnerID() == getUserGameID()) {
		Vessel* v1 = dynamic_cast<Vessel*>(getSelected());
		Outpost* o1 = dynamic_cast<Outpost*>(getSelected());

		Vessel* v2 = dynamic_cast<Vessel*>(target);
		Outpost* o2 = dynamic_cast<Outpost*>(target);


		if(o1) {
			if(v2 && !willSendWith(SpecialistType::PIRATE)) return;

			int units = getSelected()->getUnitsAt(timeDiff);

			uint32_t parameters[] = { uint32_t(std::max(selectedSpecialists.empty() ? 1 : 0, int(percent * units))), uint32_t(selected), target->getID() };
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

			while(!selectedSpecialists.empty()) {
				if(game->getSpecialist(*selectedSpecialists.begin())->getOwnerID() == userGameID) {
					arguments.push_back(*selectedSpecialists.begin());
					if(getNode(selected)) getNode(selected)->setSpecialistSelected(*selectedSpecialists.begin(), false);
				}
				selectedSpecialists.erase(selectedSpecialists.begin());
			}

			emit_signal("addOrder", "SEND", game->getReferenceID(), current, arguments);
			
			return;
		} else if(v1) {
			if(!getSelected()->controlsSpecialist(SpecialistType::NAVIGATOR)) return;

			if(v2 && !getSelected()->controlsSpecialist(SpecialistType::PIRATE)) return;

			uint32_t parameters[] = { uint32_t(selected), target->getID() };
			Array arguments;

			for(int i = 0; i < 2; i++) arguments.push_back(parameters[i]);

			emit_signal("addOrder", "REROUTE", game->getReferenceID(), current, arguments);
			
			return;
		}
	}
}

// event propagated from positional nodes, occurs when something is clicked on
void GameInterface::select(int id) {
	bool hasSpecialist = game->hasSpecialist(id);

	bool selectedSpecialist = hasSpecialist && selectedSpecialists.find(id) != selectedSpecialists.end();

	if(hasSpecialist && !selectedSpecialist) {
		setSelectedSpecialist(id);
		justSelectedSpecialist = true;
	}

	justSelect = true;

	if(getSelected() && (id == getSelected()->getID() || hasSpecialist)) {
		Vessel* v1 = dynamic_cast<Vessel*>(getSelected());

		if(v1) startDrag = willSendWith(SpecialistType::NAVIGATOR);
		else startDrag = true;
		return;
	}

	if(hasSpecialist) return;

	Vessel* v1 = dynamic_cast<Vessel*>(getObj(id));

	setSelected(id);
	
	if(v1) startDrag = willSendWith(SpecialistType::NAVIGATOR);
	else startDrag = true;
}

void GameInterface::unselect() {
	setSelected(-1);

	emit_signal("deselect");

	while(!selectedSpecialists.empty()) setSelectedSpecialist(*selectedSpecialists.begin());
}

void GameInterface::setSelected(int id) {
	if(getSelected() && getNode(selected)) getNode(selected)->setSelected(false);

	PositionalNode* obj = getNode(id);

	if(obj) {
		obj->setSelected(true);
	}

	VesselNode* v = dynamic_cast<VesselNode*>(obj);
	OutpostNode* o = dynamic_cast<OutpostNode*>(obj);
	if(v) emit_signal("selectVessel", v);
	if(o) emit_signal("selectOutpost", o);

	selected = id;
}

void GameInterface::setSelectedSpecialist(int id) {
	bool selected = selectedSpecialists.find(id) == selectedSpecialists.end();

	Specialist* s = game->getSpecialist(id);

	if(!s) {
		for(auto& pair : vessels) if(pair.second->hasSelectedSpecialist(id)) pair.second->setSpecialistSelected(id, false);
		for(auto& pair : outposts) if(pair.second->hasSelectedSpecialist(id)) pair.second->setSpecialistSelected(id, false);

		selectedSpecialists.erase(id);
		emit_signal("deselectSpecialist", id);

		return;
	}

	int containerID = s->getContainer() ? s->getContainer()->getID() : -1;
	bool owned = s->getContainer() && s->getContainer()->getOwnerID() == getUserGameID();
	PositionalNode* container = getNode(containerID);

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

	if(!owned) selectedSpecialists.clear();
	
	if(selected) {
		select(containerID);
		selectedSpecialists.insert(id);
		emit_signal("selectSpecialist", id);
	} else {
		selectedSpecialists.erase(id);
		emit_signal("deselectSpecialist", id);
		setSelected(containerID);
	}
}

void GameInterface::bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
}

void GameInterface::endBulkAdd() {
	completeGame->run();
	game = nullptr;
	simulatedGame = nullptr;
	
	current += epsilon;
	
	if(!paused) {
		currentGame = nullptr;
		update();
	}
}

void GameInterface::addOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
	completeGame->run();
	game = nullptr;
	simulatedGame = nullptr;
	currentGame = nullptr;

	current += epsilon;
	
	update();
}

void GameInterface::cancelOrder(uint32_t ID) {
	completeGame = completeGame->removeOrder(ID);
	completeGame->run();
	game = nullptr;
	simulatedGame = nullptr;
	currentGame = nullptr;
	
	update();
}

PositionalNode* GameInterface::getTarget(double x, double y) {
	PositionalNode* target = nullptr;

	Point p = Point(game->getSettings(), x, y);
	p.constrain();

	double minDist = 10.0;

	for(auto& pair : outposts) {
		if(pair.first == selected || !pair.second->is_visible()) continue;

		double mag = pair.second->getObj()->getPositionAt(pair.second->getDiff()).closestDistance(p);
		if(mag < minDist) {
			target = pair.second;
			minDist = mag;
		}
	}

	if(willSendWith(SpecialistType::PIRATE)) {
		for(auto& pair : vessels) {
			if(pair.first == selected || !pair.second->is_visible()) continue;

			double mag = pair.second->getObj()->getPositionAt(pair.second->getDiff()).closestDistance(p);
			if(mag < minDist) {
				target = pair.second;
				minDist = mag;
			}
		}
	}

	return target;
}

double GameInterface::projectedTime(double x, double y) {
	if(!getSelected()) return -1;

	double timeDiff = settings.clientToGameTime(getCurrent()) - simulatedGame->getTime();

	Point p = Point(game->getSettings(), x, y);
	p = getSelected()->getPositionAt(timeDiff).closest(p);

	PositionalNode* target = getTarget(x, y);

	double speed = getSelected()->getProjectedSpeed(target ? target->getObj() : nullptr, selectedSpecialists);

	double mag = (target ? target->getObj()->getPositionAt(target->getDiff()) : p).closestDistance(getSelected()->getPositionAt(timeDiff));

	return settings.gameToClientTime(settings.clientToGameTime(getCurrent()) + mag/speed) - getCurrent();
}

PackedVector2Array GameInterface::getOutpostPositions() {
	PackedVector2Array arr;

	double time = getTimeMillis();
	double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();
	for(auto& pair : outposts) {
		if(!pair.second->is_visible()) {
			Player* p = future ? currentGame->getPlayer(getUserGameID()) : game->getPlayer(getUserGameID());
			std::shared_ptr<Game> visibilityGame = future ? currentGame : game;

			pair.second->setDiff(time, timeDiff);
			pair.second->set_visible(visibilityGame->withinRange(p, pair.second->getObj(), timeDiff));
		}

		if(!pair.second->is_visible()) continue;
		const Point& p = pair.second->getObj()->getPositionAt(pair.second->getDiff());
		arr.push_back(Vector2(p.getX(), p.getY()));
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

Array GameInterface::getPlayers() {
	Array arr;

	for(const auto& pair : game->getPlayers()) {
		arr.push_back(players[pair.second->getID()]);
	}

	return arr;
}


Array GameInterface::getSortedPlayers() {
	Array arr;
	std::vector<Player*> sortedPlayers = game->sortedPlayers(settings.clientToGameTime(getTime()) - game->getTime());

	for(Player* player_ : sortedPlayers) {
		arr.push_back(players[player_->getID()]);
	}

	return arr;
}

Array GameInterface::getCurrentSortedPlayers() {
	Array arr;
	std::vector<Player*> sortedPlayers = currentGame->sortedPlayers(settings.clientToGameTime(getTimeMillis()) - game->getTime());

	for(Player* p : sortedPlayers) {
		arr.push_back(players[p->getID()]);
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

PackedInt32Array GameInterface::getAllSpecialists() {
	PackedInt32Array arr;

	for(SpecialistType t : Specialist::allHires()) {
		arr.push_back(uint32_t(t));
	}

	return arr;
}

PackedInt32Array GameInterface::getShopOptions() {
	PackedInt32Array arr;

	for(SpecialistType t : Specialist::baseHires()) {
		if(settings.specialistBans.find(t) == settings.specialistBans.end()) arr.push_back(uint32_t(t));
	}

	return arr;
}

PackedInt32Array GameInterface::getPromotionOptions(int specialistNum) {
	PackedInt32Array arr;

	for(SpecialistType t : Specialist::promotionOptions(SpecialistType(specialistNum))) {
		if(settings.specialistBans.find(t) == settings.specialistBans.end()) arr.push_back(uint32_t(t));
	}

	return arr;
}

String GameInterface::getSpecialistName(int specialistNum) {
	return String(Specialist::typeAsString(SpecialistType(specialistNum)).c_str());
}

String GameInterface::getSpecialistDescription(int specialistNum) {
	return String(settings.specialistDescriptions[SpecialistType(specialistNum)].c_str());
}

int GameInterface::getSpecialistHireAmount(int specialistNum) {
	return Specialist::hireAmount(SpecialistType(specialistNum));
}

String GameInterface::getNextVictoryMessage() {
	double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();

	Player* victor = game->sortedPlayers(timeDiff).front();
	std::string res = victor->getName();

	if(settings.gameMode == Mode::CONQUEST) {
		if(settings.outpostsToWin - victor->getOutposts().size() > 0 && !game->simulationEnded()) res += " needs " + std::to_string((1 + (game->getPlayers().size() / 2)) * settings.outpostsPerPlayer - victor->getOutposts().size()) + " more outposts to win";
		else res += " has won";
	} else if(settings.gameMode == Mode::MINING) {
		const WinConditionEvent* e = completeGame->nextWinCondition(clientToGameTime(getTime()));

		if(e) {
			victor = game->getPlayer(e->getPlayerID());
			res = victor->getName();
		} else if(!completeGame->simulationEnded()) return "";

		if(e && settings.resourcesToWin - victor->getResourcesAt(timeDiff) > 0 && !game->simulationEnded()) {
			char str[16];
			sprintf(str, "%.2lf", (e->getTimestamp() - getTime()) / (60 * 60));
			res += " needs " + std::to_string(settings.resourcesToWin - victor->getResourcesAt(timeDiff)) + " more resources and will win in " + std::string(str) + " hours";
		} else res += " has won";
	} else {
		if(game->simulationEnded()) res += " has won";
		else return "";
	}

	return String(res.c_str());
}

double GameInterface::getNextVictoryTime() {
	const WinConditionEvent* e = completeGame->nextWinCondition(clientToGameTime(getTime()));

	return e ? settings.gameToClientTime(e->getTimestamp()) : -1;
}

PlayerNode* GameInterface::getNextVictoryPlayer() {
	const WinConditionEvent* e = completeGame->nextWinCondition(clientToGameTime(getTime()));

	return e ? players[e->getPlayerID()] : nullptr;
}

double GameInterface::getNextArrivalEvent(int vesselID) {
	const VesselOutpostEvent* e = completeGame->nextArrival(vesselID, clientToGameTime(getTime()));

	return e ? settings.gameToClientTime(e->getTimestamp()) : -1;
}

double GameInterface::getNextProductionEvent(int outpostID) {
	std::shared_ptr<Game> curr = game;

	double next = completeGame->nextState(curr->getTime());

	double time = curr->getTime();

	while(curr->getTime() + curr->getOutpost(outpostID)->nextProductionEvent(0) > next) {
		curr = completeGame->lastState(next);
		time = next;
		next = completeGame->nextState(time);
	}

	return settings.gameToClientTime(time + curr->getOutpost(outpostID)->nextProductionEvent(clientToGameTime(getTime()) - time) + epsilon);
}

double GameInterface::getNextHireEvent() {
	std::shared_ptr<Game> curr = game;

	double next = completeGame->nextState(curr->getTime());

	double time = curr->getTime();

	while(curr->getTime() + curr->getPlayer(userGameID)->nextHireEvent(0) > next) {
		curr = completeGame->lastState(next);
		time = next;
		next = completeGame->nextState(time);
	}

	return settings.gameToClientTime(time + curr->getPlayer(userGameID)->nextHireEvent(clientToGameTime(getTime()) - time) + epsilon);
}

double GameInterface::getNextBattleEvent(int objID) {
	const BattleEvent* e = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	return e ? settings.gameToClientTime(e->getTimestamp()) : -1;
}

bool GameInterface::canViewNextBattle(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	if(!b) return false;

	double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();

	Player* p = future ? currentGame->getPlayer(getUserGameID()) : game->getPlayer(getUserGameID());
	std::shared_ptr<Game> visibilityGame = future ? currentGame : game;
	
	std::pair<int, int> pair = b->getBattleObjects();

	if(!getObj(pair.first) || !visibilityGame->withinRange(p, getObj(pair.first), timeDiff)) return false;

	if(!getObj(pair.second) || !visibilityGame->withinRange(p, getObj(pair.second), timeDiff)) return false;

	return true;
}

Array GameInterface::getBattlePhases() {
	Array arr;

	for(const std::string& str : BattleEvent::getPhases()) {
		arr.push_back(String(str.c_str()));
	}

	return arr;
}

Array GameInterface::getNextBattleUsers(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Array arr;

	if(!b) return arr;
	
	if(players.find(b->getBattleUsers().first) != players.end()) arr.push_back(players[b->getBattleUsers().first]);
	if(players.find(b->getBattleUsers().second) != players.end()) arr.push_back(players[b->getBattleUsers().second]);

	return arr;
}

Array GameInterface::getNextBattleMessages(int objID, const String& phase) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

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
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getStartingUnits();

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}

Dictionary GameInterface::getNextBattleUnits(int objID, const String& phase) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getPhaseUnits(std::string(phase.utf8().get_data()));

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}


Dictionary GameInterface::getNextBattlePreVictoryUnits(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getPreVictoryUnits();

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}

Dictionary GameInterface::getNextBattleShields(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Dictionary d;

	if(!b) return d;

	std::unordered_map<int, int> units = b->getShields();

	for(auto& pair : units) {
		d[pair.first] = pair.second;
	}

	return d;
}

PlayerNode* GameInterface::getNextBattleVictor(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	if(!b) return nullptr;

	return b->getVictor() >= 0 ? players[b->getVictor()] : nullptr;
}

int GameInterface::getNextBattleVictorUnits(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	if(!b) return -1;

	return b->getVictorUnits();
}

Array GameInterface::getNextBattleCaptures(int objID) {
	const BattleEvent* b = completeGame->nextBattle(objID, clientToGameTime(getTime()));

	Array arr;

	if(!b) return arr;

	for(std::string &name : b->getCaptures()) {
		arr.push_back(String(name.c_str()));
	}

	return arr;
}

PlayerNode* GameInterface::getPlayer(int id) {
	return players[id];
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

