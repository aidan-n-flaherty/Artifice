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
#include <godot_cpp/variant/utility_functions.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <utility>
#include <map>
#include <list>
#include <unordered_set>
#include <sstream>
#include <string>

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
	ClassDB::bind_method(D_METHOD("startAtBeginning"), &GameInterface::startAtBeginning);
	ClassDB::bind_method(D_METHOD("setTempTime", "t"), &GameInterface::setTempTime);
	ClassDB::bind_method(D_METHOD("setTime", "t"), &GameInterface::setTime);
	ClassDB::bind_method(D_METHOD("getTime"), &GameInterface::getTime);
	ClassDB::bind_method(D_METHOD("setCurrent"), &GameInterface::setCurrent);
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
	ClassDB::bind_method(D_METHOD("isOffline"), &GameInterface::isOffline);
	ClassDB::bind_method(D_METHOD("getNextOfflineOrder"), &GameInterface::getNextOfflineOrder);

	//Singleplayer functions
	ClassDB::bind_method(D_METHOD("setSandbox"), &GameInterface::setSandbox);
	ClassDB::bind_method(D_METHOD("setSingleplayer"), &GameInterface::setSingleplayer);

	// battles
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

	// players
	ClassDB::bind_method(D_METHOD("getPlayerIDs"), &GameInterface::getPlayerIDs);
	ClassDB::bind_method(D_METHOD("getPlayers"), &GameInterface::getPlayers);
	ClassDB::bind_method(D_METHOD("getPlayer"), &GameInterface::getPlayer);
	ClassDB::bind_method(D_METHOD("getSortedPlayers"), &GameInterface::getSortedPlayers);
	ClassDB::bind_method(D_METHOD("getCurrentSortedPlayers"), &GameInterface::getCurrentSortedPlayers);
	ClassDB::bind_method(D_METHOD("getScore", "userID"), &GameInterface::getScore);
	ClassDB::bind_method(D_METHOD("getColor", "userID"), &GameInterface::getColor);

	// specialists
	ClassDB::bind_method(D_METHOD("canHire"), &GameInterface::canHire);
	ClassDB::bind_method(D_METHOD("canRelease", "specialistID"), &GameInterface::canRelease);
	ClassDB::bind_method(D_METHOD("canPromote", "specialistID"), &GameInterface::canPromote);
	ClassDB::bind_method(D_METHOD("canUndoSpecialist", "specialistID"), &GameInterface::canUndoSpecialist);
	ClassDB::bind_method(D_METHOD("getSpecialistOriginatingOrder", "specialistID"), &GameInterface::getSpecialistOriginatingOrder);
	ClassDB::bind_method(D_METHOD("getSpecialistOriginatingOrderType", "specialistID"), &GameInterface::getSpecialistOriginatingOrderType);
	ClassDB::bind_method(D_METHOD("getAllSpecialists"), &GameInterface::getAllSpecialists);
	ClassDB::bind_method(D_METHOD("getShopOptions"), &GameInterface::getShopOptions);
	ClassDB::bind_method(D_METHOD("getPromotionOptions"), &GameInterface::getPromotionOptions);
	ClassDB::bind_method(D_METHOD("getSpecialistName"), &GameInterface::getSpecialistName);
	ClassDB::bind_method(D_METHOD("getSpecialistDescription"), &GameInterface::getSpecialistDescription);
	ClassDB::bind_method(D_METHOD("getSpecialistHireAmount"), &GameInterface::getSpecialistHireAmount);
	ClassDB::bind_method(D_METHOD("getSpecialistType"), &GameInterface::getSpecialistType);

	// orders
	ClassDB::bind_method(D_METHOD("getOrderIDs"), &GameInterface::getOrderIDs);
	ClassDB::bind_method(D_METHOD("getInvalidOrderIDs"), &GameInterface::getInvalidOrderIDs);
	ClassDB::bind_method(D_METHOD("getOrderTimestamp"), &GameInterface::getOrderTimestamp);
	ClassDB::bind_method(D_METHOD("getOrderDescription"), &GameInterface::getOrderDescription);
	ClassDB::bind_method(D_METHOD("canUndoOrder"), &GameInterface::canUndoOrder);

	ClassDB::bind_method(D_METHOD("canGift", "vesselID"), &GameInterface::canGift);

	ClassDB::bind_method(D_METHOD("getNumTeams"), &GameInterface::getNumTeams);
	ClassDB::bind_method(D_METHOD("getFloorDisplay"), &GameInterface::getFloorDisplay);
	ClassDB::bind_method(D_METHOD("getNode", "id"), &GameInterface::getNode);
	ClassDB::bind_method(D_METHOD("getSpawnLocation"), &GameInterface::getSpawnLocation);
	ClassDB::bind_method(D_METHOD("shiftToTime", "t"), &GameInterface::shiftToTime);
	ClassDB::bind_method(D_METHOD("bulkAddOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::bulkAddOrder);
	ClassDB::bind_method(D_METHOD("endBulkAdd"), &GameInterface::endBulkAdd);
	ClassDB::bind_method(D_METHOD("addOrder", "type", "ID", "referenceID", "timestamp", "senderID", "arguments", "argCount"), &GameInterface::addOrder);

	ClassDB::bind_method(D_METHOD("cancelOrder", "ID"), &GameInterface::cancelOrder);
	ClassDB::bind_method(D_METHOD("incrementSend", "orderID"), &GameInterface::incrementSend);
	ClassDB::bind_method(D_METHOD("decrementSend", "orderID"), &GameInterface::decrementSend);
	ClassDB::bind_method(D_METHOD("alterSend", "orderID"), &GameInterface::alterSend);
	ClassDB::bind_method(D_METHOD("addSpecialist", "orderID", "specialistID"), &GameInterface::addSpecialist);
	ClassDB::bind_method(D_METHOD("removeSpecialist", "orderID", "specialistID"), &GameInterface::removeSpecialist);

	ClassDB::bind_method(D_METHOD("possibleSpecialists", "orderID"), &GameInterface::possibleSpecialists);
	ClassDB::bind_method(D_METHOD("getSpecialistTypeBeforeOrder", "specialistID", "orderID"), &GameInterface::getSpecialistTypeBeforeOrder);

	ADD_SIGNAL(MethodInfo("addOrder", PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::INT, "referenceID"), PropertyInfo(Variant::INT, "timestamp"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments")));
	ADD_SIGNAL(MethodInfo("replaceOrder", PropertyInfo(Variant::INT, "ID"), PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::INT, "referenceID"), PropertyInfo(Variant::BOOL, "canceled"), PropertyInfo(Variant::INT, "timestamp"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments"), PropertyInfo(Variant::PACKED_INT32_ARRAY, "arguments")));
	ADD_SIGNAL(MethodInfo("selectVessel", PropertyInfo(Variant::OBJECT, "vessel")));
	ADD_SIGNAL(MethodInfo("selectOutpost", PropertyInfo(Variant::OBJECT, "outpost")));
	ADD_SIGNAL(MethodInfo("selectSpecialist", PropertyInfo(Variant::INT, "specialist")));
	ADD_SIGNAL(MethodInfo("deselect"));
	ADD_SIGNAL(MethodInfo("deselectSpecialist", PropertyInfo(Variant::INT, "specialist")));
	ADD_SIGNAL(MethodInfo("moveTo", PropertyInfo(Variant::FLOAT, "t")));
}

void GameInterface::suspend() {
	this->game = nullptr;
	this->fullGame = nullptr;
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
	this->fullCompleteGame = nullptr;
	this->fullGame = nullptr;

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
	
	std::map<int, std::tuple<std::string, int, int, bool>> playerMap;
	for(int i = 0; i < playerCap; i++) {
		playerMap[i] = std::make_tuple("Unclaimed", -1, 0, false);
	}

	Array ids = players.keys();
	for(int i = 0; i < ids.size(); i++) {
		if(Variant::can_convert(ids[i].get_type(), Variant::INT)) {
			int id = int(ids[i]);

			Dictionary player = Dictionary(players[ids[i]]);

			int userID = int(player["id"]);
			int rating = int(Dictionary(player["userStats"])["rating"]);
			bool bot = Dictionary(player["userStats"]).has("bot");

			UtilityFunctions::print("ID:");
			UtilityFunctions::print(id);
			UtilityFunctions::print("Is bot?");
			UtilityFunctions::print(bot);
			
			std::string name = std::string(String(player["username"]).utf8().get_data());

			playerMap[id] = std::make_tuple(name, userID, rating, bot);
		}
	}
	
	settings = loadSettings();
	settings.startTime = startTime;
	completeGame = std::shared_ptr<Game>(new Game(settings, userID, startTime, settings.clientToGameTime(startTime + simulationBuffer / settings.simulationSpeed), playerMap, seed, true));
	fullCompleteGame = std::shared_ptr<Game>(new Game(settings, userID, startTime, settings.clientToGameTime(startTime + simulationBuffer / settings.simulationSpeed), playerMap, seed, true));

	settings = *(completeGame->getSettings());

	completeGame->run();
	fullCompleteGame->run();
	nextEndState = completeGame->getNextEndState();
	nextFullEndState = fullCompleteGame->getNextEndState();

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

	if(fullCompleteGame != nullptr && fullGame != nullptr && completeGame != nullptr && game != nullptr && !isOffline()) {
		double fullDiff = settings.clientToGameTime(time) - fullGame->getTime();

		double fullTime = settings.clientToGameTime(time);

		Player* p = fullGame->getPlayer(getUserGameID());

		if(p) {
			bool anyChanged = false;

			std::unordered_set<int> collectedVessels;

			std::shared_ptr<Game> modifiedCompleteGame;
			
			for(const auto& pair : fullGame->getVessels()) {
				bool isVisible = fullGame->withinRange(p, pair.second, fullDiff);

				for(const Event* e : completeGame->getSimulatedEvents()) {
					if(!e->referencesObject(pair.first)) continue;

					// must either be in the future and simulated, or in the past and not simulated, to trigger update
					if((e->getTimestamp() > fullTime && !e->getDisabled()) || (e->getTimestamp() < fullTime && e->getDisabled())) {
						bool simulate = isVisible;

						if(simulate) {
							std::shared_ptr<Game> gameBeforeEvent = fullCompleteGame->lastStateBefore(e->getTimestamp());

							double gameDiff = e->getTimestamp() - gameBeforeEvent->getTime();

							Vessel* v = gameBeforeEvent->getVessel(pair.first);

							if(v && !fullGame->withinRange(p, v, gameDiff)) simulate = false;
						}

						// check whether the vessel is visible, don't simulate the event if it isn't
						if(!simulate) {
							collectedVessels.insert(pair.first);

							std::shared_ptr<Game> g = completeGame->setSimulateVessel(pair.first, std::max(e->getTimestamp(), fullTime), simulate);
							if(!modifiedCompleteGame || g->getTime() <= modifiedCompleteGame->getTime()) modifiedCompleteGame = g;

							anyChanged = true;
							break;
						}
					}
				}
			}

			for(const Event* e : completeGame->getSimulatedEvents()) {
				if((e->getTimestamp() > fullTime && !e->getDisabled()) || (e->getTimestamp() < fullTime && e->getDisabled())) {
					const VesselOutpostEvent* battle = dynamic_cast<const VesselOutpostEvent*>(e);
					if(battle) {
						std::shared_ptr<Game> gameBeforeEvent = fullCompleteGame->lastStateBefore(e->getTimestamp());

						double gameDiff = e->getTimestamp() - gameBeforeEvent->getTime();
						
						PositionalObject* a = gameBeforeEvent->getPosObject(battle->getAID());
						PositionalObject* b = gameBeforeEvent->getPosObject(battle->getBID());
						
						Vessel* v = dynamic_cast<Vessel*>(a);
						if(!v) v = dynamic_cast<Vessel*>(b);
						
						if(v && collectedVessels.find(v->getID()) != collectedVessels.end()) continue;
						
						if(v && a && b && (!fullGame->withinRange(p, a, gameDiff) || !fullGame->withinRange(p, b, gameDiff))) {
							collectedVessels.insert(v->getID());

							std::shared_ptr<Game> g = completeGame->setSimulateVessel(v->getID(), std::max(e->getTimestamp(), fullTime), false);
							if(!modifiedCompleteGame || g->getTime() <= modifiedCompleteGame->getTime()) modifiedCompleteGame = g;
							anyChanged = true;
						}
					}
				}
			}

			for(const auto& pair : completeGame->getIgnoredVessels()) {
				if(fullGame->removedVessel(pair.first)) {
					for(const Event* e : completeGame->getSimulatedEvents()) {
						if(!e->referencesObject(pair.first)) continue;

						if(e->getDisabled()) {
							std::shared_ptr<Game> g = completeGame->setSimulateVessel(pair.first, fullTime, true);
							if(!modifiedCompleteGame || g->getTime() <= modifiedCompleteGame->getTime()) modifiedCompleteGame = g;
							anyChanged = true;
							break;
						}
					}
				}
			}

			if(anyChanged) {
				std::cout << "Updating simulated game" << std::endl;
				completeGame = modifiedCompleteGame;
				completeGame->run();

				game = nullptr;
				simulatedGame = nullptr;
				currentGame = nullptr;

				update();
				std::cout << "Updated!" << std::endl;
			}


		}
	}
	
	if(game != nullptr && simulatedGame != nullptr && currentGame != nullptr && fullGame != nullptr) {
		double t = settings.clientToGameTime(time);

		double fullDiff = settings.clientToGameTime(time) - fullGame->getTime();

		double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();

		double simulatedDiff = settings.clientToGameTime(getCurrent()) - simulatedGame->getTime();

		Player* p = future ? currentGame->getPlayer(getUserGameID()) : game->getPlayer(getUserGameID());

		Player* fullPlayer = fullGame->getPlayer(getUserGameID());
    
		std::shared_ptr<Game> visibilityGame = future ? currentGame : game;
		
		for(const auto& pair : vessels) {
			Vessel* v = fullGame->getVessel(pair.first);

			pair.second->setDiff(t, timeDiff);
			pair.second->setSelfOwned(pair.second->getOwnerID() == userGameID && userGameID >= 0);
			
            if(future && v && fullPlayer && !fullGame->withinRange(fullPlayer, v, fullDiff)) pair.second->setInRadar(isOffline() || finished);
			else pair.second->setInRadar(isOffline() || finished || (p ? visibilityGame->withinRange(p, pair.second->getObj(), timeDiff) : false));
			
			if(pair.second->isLoaded()) pair.second->set_visible(pair.second->isInRadar() && !pair.second->getVessel()->getDisabled());
		}
		
		for(const auto& pair : outposts) {
			pair.second->setDiff(t, timeDiff);
			pair.second->setSelfOwned(pair.second->getOwnerID() == userGameID && userGameID >= 0);
			pair.second->setInRadar(isOffline() || finished || (p ? visibilityGame->withinRange(p, pair.second->getObj(), timeDiff) : false));
			pair.second->set_visible(true);
			pair.second->setViewType(isOffline() || finished || (p ? p->controlsSpecialist(SpecialistType::INTELLIGENCE_OFFICER) : false));
		}

		for(const auto& pair : players) pair.second->setDiff(t, timeDiff);

		if(selected >= 0 && getNode(selected) && getNode(selected)->isInRadar()) selectedUnits = getSelected()->getUnitsAt(simulatedDiff);
		else selectedUnits = -1;

		floorDisplay->setDiff(timeDiff, simulatedDiff, t);
		floorDisplay->queue_redraw();

		//telling the bots to make decisions every 10 seconds
		timeSinceLastBot += delta;
		if(timeSinceLastBot <= 10) return;
		timeSinceLastBot = 0;

		//making a map of all players
		std::unordered_map<int, Player*> playerList = fullGame->getPlayers();

		//updating the counter for whose turn it is to make a decision
		++currentBot;

		//resetting counter if it gets too high
		if(currentBot == (playerList.size() + 1)) currentBot = 0;

		//counting which bot we are currently looking at
		int iterCounter = 0;
		
		for(std::unordered_map<int, Player*>::iterator iter = playerList.begin(); iter != playerList.end(); iter++) {
			if( iterCounter != currentBot) {
				++iterCounter;
				continue;
			}
			else if(iter->second->isBot()) {
				UtilityFunctions::print("Giving bot a turn:");
				UtilityFunctions::print(iter->second->getUserID());
				botLogic(iter->second->getUserID());
				break;
			}
		}
	}
}

void GameInterface::setCurrent() {
	double time = getTimeMillis();
	future = true;
	if(buffer) {
		double buffTime = getBuffTime();
		current = buffTime;
	} else current = time;
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

	bool shouldUnselect = false;
	
	if(game == nullptr || time + simulationBuffer / settings.simulationSpeed > nextEndState) {
		std::cout << "Running game" << std::endl;
		if(time > nextEndState) game = nullptr;

		completeGame = completeGame->lastState(nextEndState);
		completeGame->setEndTime(nextEndState);
		completeGame->run();
		nextEndState = completeGame->getNextEndState();
	}

	if(fullGame == nullptr || time + simulationBuffer / settings.simulationSpeed > nextFullEndState) {
		std::cout << "Running full game" << std::endl;
		if(time > nextEndState) fullGame = nullptr;

		fullCompleteGame = fullCompleteGame->lastState(nextFullEndState);
		fullCompleteGame->setEndTime(nextFullEndState);
		fullCompleteGame->run();
		nextFullEndState = fullCompleteGame->getNextEndState();
	}

	if(currentGame == nullptr || timeMillis >= nextCurrentState) {
		currentGame = completeGame->lastState(timeMillis);
		nextCurrentState = completeGame->nextState(timeMillis);
	}

	if(fullGame == nullptr || timeMillis >= nextFullState) {
		fullGame = fullCompleteGame->lastState(timeMillis);
		nextFullState = fullCompleteGame->nextState(timeMillis);
	}

	if(simulatedGame == nullptr || currentTime < simulatedGame->getTime() || currentTime >= nextSimulatedState) {
		simulatedGame = completeGame->lastState(currentTime);
		nextSimulatedState = completeGame->nextState(currentTime);

		if(selected >= 0 && !getSelected()) shouldUnselect = true;

		for(auto it = selectedSpecialists.begin(); it != selectedSpecialists.end();) {
			if(!simulatedGame->hasSpecialist(*it)) {
				setSelectedSpecialist(*it);
				it = selectedSpecialists.begin();
			} else it++;
		}
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

		if(maintainSelect >= 0 && game->hasVessel(maintainSelect) && !game->getVessel(maintainSelect)->getDisabled()) {
			setSelected(maintainSelect);
			maintainSelect = -1;
		} else if(selectOrder >= 0) {
			for(const auto& pair : game->getVessels()) {
				if(pair.second->getSourceOrder() && pair.second->getSourceOrder()->getID() == selectOrder) {
					if(selected < 0 || selected != pair.first) setSelected(pair.first);
					break;
				}
			}
			
			selectOrder = -1;
		} else if(selected >= 0 && !getSelected()) shouldUnselect = true;
		else if(selected >= 0) {
			Vessel* v = dynamic_cast<Vessel*>(getSelected());

			if(v && v->getDisabled()) shouldUnselect = true;
		}
	}

	if(shouldUnselect) {
		maintainSelect = selected;
		unselect();
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
		if(s->getType() == type && s->getOwnerID() == getUserGameID() && (v || std::find(selectedSpecialists.begin(), selectedSpecialists.end(), s->getID()) != selectedSpecialists.end())) {
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
		bool selectedSpecialist = std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id) != selectedSpecialists.end();

		if(game->hasSpecialist(id) && selectedSpecialist && !justSelectedSpecialist) {
			setSelectedSpecialist(id);
		}
	}

	justSelectedSpecialist = false;
}

void GameInterface::sendTo(int id) {
	//making sure something is currently selected
	if(!getSelected()) return;

	//checking if the player is attempting to order around a bot
	//NOTE: the dummy in sandbox mode does not have the bot property
	if(game->getPlayer(getSelected()->getOwnerID())->isBot()) return;

	//if the selected object is not owned by the player and we are not in sandbox
	//mode, then deny the function
	if(getSelected()->getOwnerID() != getUserGameID() && !isOffline()) return;

	//start of the actual function
	PositionalObject* target = getObj(id);

	if(current < getStartTime()) current = getStartTime() + epsilon;

	double simulatedDiff = settings.clientToGameTime(getCurrent()) - simulatedGame->getTime();


	Vessel* v1 = dynamic_cast<Vessel*>(getSelected());
	Outpost* o1 = dynamic_cast<Outpost*>(getSelected());

	Vessel* v2 = dynamic_cast<Vessel*>(target);
	Outpost* o2 = dynamic_cast<Outpost*>(target);

	if(o1) {
		if(v2 && !willSendWith(SpecialistType::PIRATE)) return;

		int units = getSelected()->getUnitsAt(simulatedDiff);

		uint32_t parameters[] = { uint32_t(std::max(selectedSpecialists.empty() ? 1 : 0, int(percent * units))), uint32_t(selected), target->getID() };
		PackedInt32Array arguments;

		for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

		while(!selectedSpecialists.empty()) {
			if(simulatedGame->getSpecialist(*selectedSpecialists.begin())->getOwnerID() == userGameID) {
				arguments.push_back(uint32_t(*selectedSpecialists.begin()));
				if(getNode(selected)) getNode(selected)->setSpecialistSelected(*selectedSpecialists.begin(), false);
			}
			selectedSpecialists.erase(selectedSpecialists.begin());
		}

		if(getNode(selected)) getNode(selected)->clearSelectedSpecialists();

		if(!isOffline()) emit_signal("addOrder", "SEND", simulatedGame->getReferenceID(), current, arguments);
		else addOrder("SEND", getNextOfflineOrder(), simulatedGame->getReferenceID(), false, current, isOffline() ? o1->getOwnerID() : userGameID, arguments, arguments.size());

		return;
	} else if(v1) {
		if(!getSelected()->controlsSpecialist(SpecialistType::NAVIGATOR)) return;

		if(v2 && !getSelected()->controlsSpecialist(SpecialistType::PIRATE)) return;

		uint32_t parameters[] = { uint32_t(selected), target->getID() };
		PackedInt32Array arguments;

		for(int i = 0; i < 2; i++) arguments.push_back(parameters[i]);

		if(!isOffline()) emit_signal("addOrder", "REROUTE", simulatedGame->getReferenceID(), current, arguments);
		else addOrder("REROUTE", getNextOfflineOrder(), simulatedGame->getReferenceID(), false, current, isOffline() ? v1->getOwnerID() : userGameID, arguments, arguments.size());
		
		return;
	}
}


//modified version of sendTo used by bots for communicating orders
void GameInterface::botOrder(Outpost *myOp, Outpost *enemyOp, int numUnits) {
	double simulatedDiff = settings.clientToGameTime(getCurrent()) - simulatedGame->getTime();
	
	Outpost *o1 = myOp;

	Outpost *o2 = enemyOp;

	//ideally we can replace with numUnits, for now debug output complains
	// about not having enough units when using numUnits instead
	int units = myOp->getUnitsAt(simulatedDiff);

	uint32_t parameters[] = { uint32_t(std::max(selectedSpecialists.empty() ? 1 : 0, int(percent * units))), myOp->getID(), enemyOp->getID() };
	PackedInt32Array arguments;

	for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

	while(!selectedSpecialists.empty()) {
		if(simulatedGame->getSpecialist(*selectedSpecialists.begin())->getOwnerID() == userGameID) {
			arguments.push_back(uint32_t(*selectedSpecialists.begin()));
			if(getNode(selected)) getNode(selected)->setSpecialistSelected(*selectedSpecialists.begin(), false);
		}
		selectedSpecialists.erase(selectedSpecialists.begin());
	}

	if(getNode(selected)) getNode(selected)->clearSelectedSpecialists();

	if(!isOffline()) emit_signal("addOrder", "SEND", simulatedGame->getReferenceID(), current, arguments);
	else addOrder("SEND", getNextOfflineOrder(), simulatedGame->getReferenceID(), false, current, isOffline() ? o1->getOwnerID() : userGameID, arguments, arguments.size());
}

//simple helper function for updating current target of the bot
void GameInterface::updateTarget(Outpost **currentTarget, int *currentDistance, int *currentUnitAdvantage, Outpost *newTarget, int newDistance, int newUnitAdvantage) {
	*currentTarget = newTarget;
	*currentDistance = newDistance;
	*currentUnitAdvantage = newUnitAdvantage;
}


void GameInterface::conquestLogic(int id) {
	//list of all outposts and list of our outposts
	std::unordered_map<int, Outpost*> outposts = game->getOutposts();
	std::list<Outpost*> myOutposts = game->getPlayer(id)->getOutposts();

	//iterating through our outposts to try and find something to target
	for(std::list<Outpost*>::iterator i = myOutposts.begin(); i != myOutposts.end(); ++i) {
		Outpost *myOp = (*i);
		Outpost *bestTarget = NULL;
		int bestDistance;
		int bestUnitAdvantage; 

		//running through the other outposts and comparing them to best target for this outpost
		for(std::unordered_map<int, Outpost*>::iterator j = outposts.begin(); j != outposts.end(); ++j) {
			//creating pointer to current outpost
			Outpost* currentTarget = j->second;
			
			//skipping if current outpost is owned by current player
			if(currentTarget->getOwnerID() == id) continue;
			
			//calculating unit advantage and distance from current outpost
			//TODO: more advanced shield calculations
			int currentUnitAdvantage = myOp->getUnits() - currentTarget->getUnits() - currentTarget->getMaxShield();
			
			//no point in finishing current iteration if not enough units to overtake a given outpost
			if(currentUnitAdvantage <= 0) continue;
			
			int currentDistance = myOp->distance(currentTarget->getPosition());

			//if we currently lack a best target then take the first target we can beat
			if(bestTarget == NULL) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//for conquest prioritizing taking unclaimed bases over other players bases
			else if(bestTarget->hasOwner() && !currentTarget->hasOwner()) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//prioritizing unit advantage over distance for now
			else if(currentUnitAdvantage > bestUnitAdvantage) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//if everything else is equal then take the closer outpost
			else if(currentUnitAdvantage == bestUnitAdvantage && currentDistance < bestDistance) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
		}

		//if from our current outpost we have a valid target we will go after it
		if(bestTarget != NULL) {
			//leaving some units at our outpost for defense
			int numUnits = myOp->getUnits() + (bestUnitAdvantage/2);
			botOrder(myOp, bestTarget, numUnits);
			return;
		} 
	}
}

void GameInterface::miningLogic(int id) {
	//list of all outposts and list of our outposts
	std::unordered_map<int, Outpost*> outposts = game->getOutposts();
	std::list<Outpost*> myOutposts = game->getPlayer(id)->getOutposts();

	//iterating through our outposts to try and find something to target
	for(std::list<Outpost*>::iterator i = myOutposts.begin(); i != myOutposts.end(); ++i) {
		Outpost *myOp = (*i);
		Outpost *bestTarget = NULL;
		int bestDistance;
		int bestUnitAdvantage; 

		//running through the other outposts and comparing them to best target for this outpost
		for(std::unordered_map<int, Outpost*>::iterator j = outposts.begin(); j != outposts.end(); ++j) {
			//creating pointer to current outpost
			Outpost* currentTarget = j->second;
			
			//skipping if current outpost is owned by current player
			if(currentTarget->getOwnerID() == id) continue;
			
			//calculating unit advantage and distance from current outpost
			//TODO: more advanced shield calculations
			int currentUnitAdvantage = myOp->getUnits() - currentTarget->getUnits() - currentTarget->getMaxShield();
			
			//no point in finishing current iteration if not enough units to overtake a given outpost
			if(currentUnitAdvantage <= 0) continue;

			int currentDistance = myOp->distance(currentTarget->getPosition());

			//if we currently lack a best target then take the first target we can beat
			if(bestTarget == NULL) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//prioritizing mines over other outpost types 
			else if(bestTarget->getType() != OutpostType::MINE && currentTarget->getType() == OutpostType::MINE) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//prioritizing unit advantage over distance
			else if(currentUnitAdvantage > bestUnitAdvantage) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			else if(currentUnitAdvantage == bestUnitAdvantage && currentDistance < bestDistance) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
		}

		//if from our current outpost we have a valid target we will go after it
		if(bestTarget != NULL) {
			//leaving some units at our outpost for defense
			int numUnits = myOp->getUnits() + (bestUnitAdvantage/2);
			botOrder(myOp, bestTarget, numUnits);
			return;
		} 
	}
}

void GameInterface::eliminationLogic(int id) {
	//list of all outposts and list of our outposts
	std::unordered_map<int, Outpost*> outposts = game->getOutposts();
	std::list<Outpost*> myOutposts = game->getPlayer(id)->getOutposts();

	//iterating through our outposts to try and find something to target
	for(std::list<Outpost*>::iterator i = myOutposts.begin(); i != myOutposts.end(); ++i) {
		Outpost *myOp = (*i);
		Outpost *bestTarget = NULL;
		int bestDistance;
		int bestUnitAdvantage; 

		//running through the other outposts and comparing them to best target for this outpost
		for(std::unordered_map<int, Outpost*>::iterator j = outposts.begin(); j != outposts.end(); ++j) {
			//creating pointer to current outpost
			Outpost* currentTarget = j->second;
			
			//skipping if current outpost is owned by current player
			if(currentTarget->getOwnerID() == id) continue;
			
			//calculating unit advantage and distance from current outpost
			//TODO: more advanced shield calculations
			int currentUnitAdvantage = myOp->getUnits() - currentTarget->getUnits() - currentTarget->getMaxShield();
			
			//no point in finishing current iteration if not enough units to overtake a given outpost
			if(currentUnitAdvantage <= 0) continue;

			int currentDistance = myOp->distance(currentTarget->getPosition());

			//if we currently lack a best target then take the first target we can beat
			if(bestTarget == NULL) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//prioritizing owned outposts over unowned outposts
			else if(!bestTarget->hasOwner() && currentTarget->hasOwner()) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			//prioritizing unit advantage over distance
			else if(currentUnitAdvantage > bestUnitAdvantage) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
			else if(currentUnitAdvantage == bestUnitAdvantage && currentDistance < bestDistance) {
				updateTarget(&bestTarget, &bestDistance, &bestUnitAdvantage, 
				currentTarget, currentDistance, currentUnitAdvantage);
			}
		}

		//if from our current outpost we have a valid target we will go after it
		if(bestTarget != NULL) {
			//leaving some units at our outpost for defense
			int numUnits = myOp->getUnits() + (bestUnitAdvantage/2);
			botOrder(myOp, bestTarget, numUnits);
			return;
		} 
	}
}

//pass in the id of the bot and the bot will choose to make a decision
void GameInterface::botLogic(int id) {

	//for some unknown reason all singleplayer gamemodes are currently seen as mining
	if(settings.gameMode == Mode::CONQUEST) {
		UtilityFunctions::print("conquest logic conquest");
		conquestLogic(id);
	}
	if(settings.gameMode == Mode::MINING) {
		UtilityFunctions::print("mining logic selected");
		miningLogic(id);
	}
	if(settings.gameMode == Mode::ELIMINATION) {
		UtilityFunctions::print("elimination logic selected");
		eliminationLogic(id);
	}
}

// event propagated from positional nodes, occurs when something is clicked on
void GameInterface::select(int id) {
	maintainSelect = -1;

	bool hasSpecialist = game->hasSpecialist(id);

	bool selectedSpecialist = hasSpecialist && std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id) != selectedSpecialists.end();

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
	bool selected = std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id) == selectedSpecialists.end();

	Specialist* s = simulatedGame->getSpecialist(id);

	if(!s) {
		for(auto& pair : vessels) if(pair.second->hasSelectedSpecialist(id)) pair.second->setSpecialistSelected(id, false);
		for(auto& pair : outposts) if(pair.second->hasSelectedSpecialist(id)) pair.second->setSpecialistSelected(id, false);

		if(!selected) selectedSpecialists.erase(std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id));
		emit_signal("deselectSpecialist", id);

		return;
	}

	int containerID = s->getContainer() ? s->getContainer()->getID() : -1;
	bool owned = s->getContainer() && s->getContainer()->getOwnerID() == getUserGameID();
	PositionalNode* container = getNode(containerID);

	for(auto& pair : vessels) if(pair.first != containerID || (!owned && !isOffline())) pair.second->clearSelectedSpecialists();
	for(auto& pair : outposts) if(pair.first != containerID || (!owned && !isOffline())) pair.second->clearSelectedSpecialists();

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
		selectedSpecialists.push_back(id);
		if(selectedSpecialists.size() > 3) setSelectedSpecialist(*selectedSpecialists.begin());
		emit_signal("selectSpecialist", id);
	} else {
		if(std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id) != selectedSpecialists.end()) selectedSpecialists.erase(std::find(selectedSpecialists.begin(), selectedSpecialists.end(), id));
		emit_signal("deselectSpecialist", id);
		setSelected(containerID);
	}
}

void GameInterface::bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
	fullCompleteGame = fullCompleteGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
}

void GameInterface::endBulkAdd() {
	fullCompleteGame->run();
	completeGame->run();

	game = nullptr;
	simulatedGame = nullptr;
	
	current += epsilon;
	
	if(!paused) {
		fullGame = nullptr;
		currentGame = nullptr;
		update();
	}
}

void GameInterface::addOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount) {
	int arr[argCount];
	for(int i = 0; i < argCount; i++) arr[i] = arguments[i];

	if(type == "SEND") selectOrder = ID;

	fullCompleteGame = fullCompleteGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
	fullCompleteGame->run();
	fullGame = nullptr;

	completeGame = completeGame->processOrder(std::string(type.utf8().get_data()), ID, referenceID, canceled, settings.clientToGameTime(timestamp), senderID, arr, argCount);
	completeGame->run();
	currentGame = nullptr;

	game = nullptr;
	simulatedGame = nullptr;

	if(current < gameToClientTime(completeGame->getStartTime())) current = timestamp + epsilon;
	else current += epsilon;
	
	update();
}

void GameInterface::cancelOrder(uint32_t ID) {
	fullCompleteGame = fullCompleteGame->removeOrder(ID);
	fullCompleteGame->run();
	fullGame = nullptr;

	completeGame = completeGame->removeOrder(ID);
	completeGame->run();
	currentGame = nullptr;

	simulatedGame = nullptr;
	game = nullptr;
	
	update();
}

void GameInterface::incrementSend(int orderID) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(order) {
		std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

		Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

		if(outpost && outpost->getUnitsAt(order->getTimestamp() - gameAtOrder->getTime()) >= order->getUnits() + 1) {
			uint32_t parameters[] = { uint32_t(order->getUnits() + 1), uint32_t(order->getOriginID()), uint32_t(order->getTargetID()) };
			Array oldArguments;
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);
			for(int id : order->getSpecialistIDs()) arguments.push_back(uint32_t(id));
			for(int id : order->getSpecialistIDs()) oldArguments.push_back(uint32_t(id));

			if(!isOffline()) emit_signal("replaceOrder", orderID, "SEND", order->getReferenceID(), order->isCanceled(), order->getTimestamp(), arguments, oldArguments);
			addOrder("SEND", orderID, order->getReferenceID(), order->isCanceled(), order->getTimestamp(), userGameID, arguments, arguments.size());
		}
	}
}

void GameInterface::decrementSend(int orderID) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(order) {
		std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

		Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

		if(outpost && order->getUnits() > 1) {
			uint32_t parameters[] = { uint32_t(order->getUnits() - 1), uint32_t(order->getOriginID()), uint32_t(order->getTargetID()) };
			Array oldArguments;
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);
			for(int id : order->getSpecialistIDs()) arguments.push_back(uint32_t(id));
			for(int id : order->getSpecialistIDs()) oldArguments.push_back(uint32_t(id));

			if(!isOffline()) emit_signal("replaceOrder", orderID, "SEND", order->getReferenceID(), order->isCanceled(), order->getTimestamp(), arguments, oldArguments);
			addOrder("SEND", orderID, order->getReferenceID(), order->isCanceled(), order->getTimestamp(), userGameID, arguments, arguments.size());
		}
	}
}

void GameInterface::alterSend(int orderID, int units) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(order) {
		std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

		Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

		if(outpost) {
			if(units < 1) units = 1;
			if(units > outpost->getUnitsAt(order->getTimestamp() - gameAtOrder->getTime())) units = outpost->getUnitsAt(order->getTimestamp() - gameAtOrder->getTime());
			uint32_t parameters[] = { uint32_t(units), uint32_t(order->getOriginID()), uint32_t(order->getTargetID()) };
			Array oldArguments;
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);
			for(int id : order->getSpecialistIDs()) arguments.push_back(uint32_t(id));
			for(int id : order->getSpecialistIDs()) oldArguments.push_back(uint32_t(id));

			if(!isOffline()) emit_signal("replaceOrder", orderID, "SEND", order->getReferenceID(), order->isCanceled(), order->getTimestamp(), arguments, oldArguments);
			addOrder("SEND", orderID, order->getReferenceID(), order->isCanceled(), order->getTimestamp(), userGameID, arguments, arguments.size());
		}
	}
}

void GameInterface::addSpecialist(int orderID, int specialistID) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(order) {
		std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

		Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

		if(outpost) {
			uint32_t parameters[] = { uint32_t(order->getUnits()), uint32_t(order->getOriginID()), uint32_t(order->getTargetID()) };
			Array oldArguments;
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);

			std::list<int> specialistsToSend;
			for(int id : order->getSpecialistIDs()) if(id != specialistID) specialistsToSend.push_back(id);
			for(int id : order->getSpecialistIDs()) oldArguments.push_back(uint32_t(id));
			if(gameAtOrder->hasSpecialist(specialistID)) specialistsToSend.push_back(specialistID);
			while(specialistsToSend.size() > 3) specialistsToSend.pop_front();

			for(int id : specialistsToSend) arguments.push_back(uint32_t(id));

			if(!isOffline()) emit_signal("replaceOrder", orderID, "SEND", order->getReferenceID(), order->isCanceled(), order->getTimestamp(), arguments, oldArguments);
			addOrder("SEND", orderID, order->getReferenceID(), order->isCanceled(), order->getTimestamp(), userGameID, arguments, arguments.size());
		}
	}
}

void GameInterface::removeSpecialist(int orderID, int specialistID) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(order) {
		std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

		Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

		if(outpost) {
			uint32_t parameters[] = { uint32_t(order->getUnits()), uint32_t(order->getOriginID()), uint32_t(order->getTargetID()) };
			Array oldArguments;
			Array arguments;

			for(int i = 0; i < 3; i++) arguments.push_back(parameters[i]);
			for(int id : order->getSpecialistIDs()) if(id != specialistID) arguments.push_back(uint32_t(id));
			for(int id : order->getSpecialistIDs()) oldArguments.push_back(uint32_t(id));

			if(!isOffline()) emit_signal("replaceOrder", orderID, "SEND", order->getReferenceID(), order->isCanceled(), order->getTimestamp(), arguments, oldArguments);
			addOrder("SEND", orderID, order->getReferenceID(), order->isCanceled(), order->getTimestamp(), userGameID, arguments, arguments.size());
		}
	}
}

Array GameInterface::possibleSpecialists(int orderID) {
	Array arr;

	Order* o = completeGame->getOrder(orderID);

	if(!o) return arr;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(!order) return arr;

	std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

	Outpost* outpost = gameAtOrder->getOutpost(order->getOriginID());

	if(outpost) {
		for(Specialist* s : outpost->getSpecialists()) arr.push_back(s->getID());
	}

	return arr;
}

int GameInterface::getSpecialistTypeBeforeOrder(int specialistID, int orderID) {
	Order* o = completeGame->getOrder(orderID);

	if(!o) return 0;

	SendOrder* order = dynamic_cast<SendOrder*>(o);

	if(!order) return 0;

	std::shared_ptr<Game> gameAtOrder = completeGame->stateBefore(orderID);

	return gameAtOrder->hasSpecialist(specialistID) ? gameAtOrder->getSpecialist(specialistID)->getType() : 0;
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
			if(pair.first == selected || !pair.second->is_visible() || pair.second->getVessel()->getDisabled()) continue;

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

	std::set<int> specialists(selectedSpecialists.begin(), selectedSpecialists.end());

	double speed = getSelected()->getProjectedSpeed(target ? target->getObj() : nullptr, specialists);

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
			pair.second->set_visible(true);
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

PackedInt32Array GameInterface::getOrderIDs() {
	PackedInt32Array arr;

	for(const auto& order : game->getOrders()) {
		if(order->getSenderID() == getUserGameID() && !order->isCanceled()) arr.push_back(uint32_t(order->getID()));
	}

	return arr;
}

PackedInt32Array GameInterface::getInvalidOrderIDs() {
	PackedInt32Array arr;

	for(const auto& order : completeGame->getInvalid()) {
		if(order->getSenderID() == getUserGameID() && !order->isCanceled()) arr.push_back(uint32_t(order->getID()));
	}

	return arr;
}

Array GameInterface::getPlayers() {
	Array arr;

	if(!game) return arr;

	for(const auto& pair : game->getPlayers()) {
		arr.push_back(players[pair.second->getID()]);
	}

	return arr;
}


Array GameInterface::getSortedPlayers() {
	Array arr;

	if(!game) return arr;

	std::vector<Player*> sortedPlayers = game->sortedPlayers(settings.clientToGameTime(getTime()) - game->getTime());

	for(Player* player_ : sortedPlayers) {
		arr.push_back(players[player_->getID()]);
	}

	return arr;
}

Array GameInterface::getCurrentSortedPlayers() {
	Array arr;

	if(!currentGame) return arr;

	std::vector<Player*> sortedPlayers = currentGame->sortedPlayers(settings.clientToGameTime(getTimeMillis()) - game->getTime());

	for(Player* p : sortedPlayers) {
		arr.push_back(players[p->getID()]);
	}

	return arr;
}

int GameInterface::getScore(int userID) {
	if(!game) return -1;

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
	if(!game) return "";

	double timeDiff = settings.clientToGameTime(getTime()) - game->getTime();

	Player* victor = game->sortedPlayers(timeDiff).front();
	std::string res = victor->getName();

	if(settings.gameMode == Mode::CONQUEST) {
		if(settings.outpostsToWin - victor->getOutposts().size() > 0) res += " needs " + std::to_string((1 + (game->getPlayers().size() / 2)) * settings.outpostsPerPlayer - victor->getOutposts().size()) + " more outposts to win";
		else if(!game->simulationEnded()) return "";
		else res += " has won";
	} else if(settings.gameMode == Mode::MINING) {
		const WinConditionEvent* e = completeGame->nextWinCondition(clientToGameTime(getTime()));

		if(e) {
			victor = game->getPlayer(e->getPlayerID());
			res = victor->getName();
		} else if(!game->simulationEnded()) return "";

		if(e && victor && settings.resourcesToWin - victor->getResourcesAt(timeDiff) > 0 && !game->simulationEnded()) {
			char str[16];
			sprintf(str, "%.2lf", ((e->getTimestamp() - clientToGameTime(getTime())) / (60 * 60)));
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
	if(!game->hasPlayer(userGameID)) return -1;
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

	if(getObj(pair.first) && p && !visibilityGame->withinRange(p, getObj(pair.first), timeDiff)) return false;

	if(getObj(pair.second) && p && !visibilityGame->withinRange(p, getObj(pair.second), timeDiff)) return false;

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

double GameInterface::getOrderTimestamp(int orderID) {
	const Order* o = completeGame->getOrder(orderID);

	return o ? settings.gameToClientTime(o->getTimestamp()) : -1;
}

String GameInterface::getOrderDescription(int orderID) {
	const Order* o = completeGame->getOrder(orderID);

	return String(o ? o->getDescription().c_str() : "");
}

bool GameInterface::canUndoOrder(int orderID) {
	double t = settings.clientToGameTime(getTimeMillis());

	const Order* o = completeGame->getOrder(orderID);

	return o && o->getTimestamp() > t;
}
