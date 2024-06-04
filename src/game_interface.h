#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <godot_cpp/classes/node3d.hpp>
#include <list>
#include <tuple>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include "../GameLogic/gameClasses/game.h"
#include "../GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameLogic/gameClasses/gameObjects/specialist.h"
#include "../GameLogic/gameClasses/game_settings.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include "player_node.h"
#include "floor_display.h"

namespace godot {

class GameInterface : public Node3D {
    GDCLASS(GameInterface, Node3D)

private:
	// stores all future game states
	std::shared_ptr<Game> completeGame = nullptr;

	// game at the current time
	std::shared_ptr<Game> game = nullptr;
	double nextState = 0.0;
	double nextEndState = 0.0;

	// game at the start of drag
	std::shared_ptr<Game> currentGame = nullptr;
	double nextCurrentState = 0.0;

	// game at the end of drag
	std::shared_ptr<Game> simulatedGame = nullptr;
	double nextSimulatedState = 0.0;
	
	std::unordered_map<int, VesselNode*> vessels;
	std::unordered_map<int, OutpostNode*> outposts;
	std::unordered_map<int, PlayerNode*> players;

	FloorDisplay* floorDisplay = nullptr;

	Dictionary settingOverrides;

	GameSettings settings;
	
	int gameID = -1; 

	int userID = -1, userGameID = -1;
	
	int selected = -1;

	int selectedUnits = -1;
	
	std::set<int> selectedSpecialists;
	
	double current = 0.0;

	double tempTime = 0.0;
	
	bool future = true;

	bool buffer = true;

	double simulationBuffer = 96 * 60 * 60;

	double percent = 1.0;

	double epsilon = 0.00001;

	bool drag = false;

	bool dragged = false;

	bool startDrag = false;

	bool justSelect = false;

	bool justSelectedSpecialist = false;

	bool finished = false;

	bool paused = false;

	Point mouse;

protected:
	static void _bind_methods();

public:
    GameInterface() {
		floorDisplay = memnew(FloorDisplay(this));
	}

    ~GameInterface() {}

	void init(int gameID, int userID, int seed, int startTime, bool finished, int playerCap, Dictionary players, Dictionary settingOverrides);

	void suspend();

	void resume();

	GameSettings loadSettings();
		
    void _process(double delta) override;
		
	void update();

	FloorDisplay* getFloorDisplay() { return floorDisplay; }

	std::shared_ptr<Game> getCompleteGame() { return completeGame; }
	std::shared_ptr<Game> getGame() { return game; }
	std::shared_ptr<Game> getSimulatedGame() { return currentGame; }
	std::shared_ptr<Game> getCurrentGame() { return currentGame; }

	int getUserGameID() { return userGameID; }

	int getHires() {
		double timeDiff = settings.clientToGameTime(current) - game->getTime();
		return game && game->hasPlayer(userGameID) ? game->getPlayer(userGameID)->getHiresAt(timeDiff) : -1;
	}
	
	double getTimeMillis();
	double getBuffTime() { return buffer ? getTimeMillis() + 10 * 60 / settings.simulationSpeed : getTimeMillis(); }
	void setBuff(bool value) { buffer = value; }
	bool getBuff() { return buffer; }
	bool simulatingFuture() { return future; }
	bool willSendWith(SpecialistType type);
	void setSelectedSpecialist(int id);
	void select(int id);
	void sendTo(int id);
	void release(int id);
	void setSelected(int id);
	void unselect();
	bool canStartDrag() { return startDrag; }
	bool justSelected() { return justSelect; }
	void setMouse(double x, double y) {
		mouse = Point(game->getSettings(), x, y);
		if(getSelected()) mouse = getSelected()->getPosition().closest(mouse);
		else mouse.constrain();
	}
	const Point& getMouse() { return mouse; }
	void setDrag(bool drag) {
		this->drag = drag;
		if(drag) this->dragged = true;
		else this->startDrag = this->justSelect = false;
	}
	bool isDragging() { return drag; }; 
	int getSelectedUnits() { return selected != -1 ? selectedUnits : -1; }
	PositionalObject* getSelected() { return getObj(selected); }
	PositionalObject* getObj(int id) { return simulatedGame->getPosObject(id); }
	PositionalNode* getNode(int id);
	
	void startAtEnd() { if(completeGame && completeGame->hasEnded()) setTime(completeGame->getGameEndTime()); };
	void shiftToTime(double t);
	void setTime(double t);
	void setTempTime(double t) { tempTime = t; }
	double getTime() { return current + tempTime; }
	double getCurrent() { return current; }
	double getClientTime() { return settings.gameToClientTime(getTime()); }
	double clientToGameTime(double t) { return settings.clientToGameTime(t); }
	double gameToClientTime(double t) { return settings.gameToClientTime(t); }

	void setPercent(double percent) { this->percent = percent; }
	double getPercent() { return percent; }

	bool isMining() { return settings.gameMode == Mode::MINING; }
	bool isConquest() { return settings.gameMode == Mode::CONQUEST; }
	bool isElimination() { return settings.gameMode == Mode::ELIMINATION; }

	Vector2 getSpawnLocation() { return game && game->hasPlayer(userGameID) && game->getPlayer(userGameID)->getSpawnLocation()
		? Vector2(game->getPlayer(userGameID)->getSpawnLocation()->getPositionAt(settings.clientToGameTime(getTime()) - game->getTime()).getX(),
				game->getPlayer(userGameID)->getSpawnLocation()->getPositionAt(settings.clientToGameTime(getTime()) - game->getTime()).getY()) : Vector2(0.0, 0.0); }
	PositionalNode* getTarget(double x, double y);
	double projectedTime(double x, double y);
	PackedInt32Array getPlayerIDs();
	PackedInt32Array getOrderIDs();
	PackedInt32Array getInvalidOrderIDs();
	PackedVector2Array getOutpostPositions();
	PackedInt32Array getAllSpecialists();
	PackedInt32Array getShopOptions();
	PackedInt32Array getPromotionOptions(int specialistNum);
	Array getPlayers();
	Array getSortedPlayers();
	Array getCurrentSortedPlayers();
	int getScore(int userID);
	Color getColor(int userID);

	double getOrderTimestamp(int orderID);
	String getOrderDescription(int orderID);
	bool canUndoOrder(int orderID);

	int getSpecialistType(int specialistID) { return game->hasSpecialist(specialistID) ? game->getSpecialist(specialistID)->getType() : 0; };
	PlayerNode* getPlayer(int id);
	PlayerNode* getSpecialistOwner(int specialistID) { return game->hasSpecialist(specialistID) && game->hasPlayer(game->getSpecialist(specialistID)->getOwnerID()) ? getPlayer(game->getSpecialist(specialistID)->getOwnerID()) : nullptr; }
	String getSpecialistName(int specialistNum);
	String getSpecialistDescription(int specialistNum);
	int getSpecialistHireAmount(int specialistNum);
	bool canRelease(int specialistID) {
		return game->hasSpecialist(specialistID) && !ownsSpecialist(specialistID) && game->getSpecialist(specialistID)->getContainer() &&
				game->getSpecialist(specialistID)->getContainer()->getOwnerID() == userGameID;
	}
	bool canPromote(int specialistID) {
		return ownsSpecialist(specialistID) && game->getSpecialist(specialistID)->getContainer() &&
			game->getSpecialist(specialistID)->getContainer()->getOwnerID() == userGameID &&
				dynamic_cast<Outpost*>(game->getSpecialist(specialistID)->getContainer());
	}
	bool canUndoSpecialist(int specialistID) { return game->hasSpecialist(specialistID) && game->getSpecialist(specialistID)->getOriginatingOrder() && game->getSpecialist(specialistID)->getOriginatingOrder()->getTimestamp() > settings.clientToGameTime(getTimeMillis()); }
	int getSpecialistOriginatingOrder(int specialistID) { return canUndoSpecialist(specialistID) ? game->getSpecialist(specialistID)->getOriginatingOrder()->getID() : -1; }
	String getSpecialistOriginatingOrderType(int specialistID) { return canUndoSpecialist(specialistID) ? String(game->getSpecialist(specialistID)->getOriginatingOrder()->getType().c_str()) : ""; }
	bool ownsSpecialist(int specialistID) { return game->hasSpecialist(specialistID) && game->getSpecialist(specialistID)->getOwnerID() == userGameID; }
	bool ownsObj(int objID) { return game->hasPosObject(objID) && game->getPosObject(objID)->getOwnerID() == userGameID; }


    String getNextVictoryMessage();
	double getNextVictoryTime();
	PlayerNode* getNextVictoryPlayer();
	double getNextArrivalEvent(int vesselID);
	double getNextProductionEvent(int outpostID);
	double getNextHireEvent();
	double getNextBattleEvent(int objID);
	bool canViewNextBattle(int objID);
	Array getBattlePhases();
	Array getNextBattleUsers(int objID);
	Array getNextBattleMessages(int objID, const String& phase);
	Dictionary getNextBattleStartingUnits(int objID);
	Dictionary getNextBattleUnits(int objID, const String& phase);
	Dictionary getNextBattlePreVictoryUnits(int objID);
	Dictionary getNextBattleShields(int objID);
	PlayerNode* getNextBattleVictor(int objID);
	int getNextBattleVictorUnits(int objID);
	Array getNextBattleCaptures(int objID);

	bool canHire() { return game && game->hasPlayer(userGameID) && game->getPlayer(userGameID)->getHiresAt(settings.clientToGameTime(getCurrent()) - game->getTime()) >= 0 && dynamic_cast<Outpost*>(game->getPlayer(userGameID)->getSpawnLocation()) && game->getPlayer(userGameID)->getSpawnLocation()->getOwnerID() == userGameID && future; }
	bool hasStarted() { return current >= game->getStartTime(); }
	bool hasEnded() { return game->hasEnded() && currentGame->hasEnded(); }
	bool hasLost() { return game && game->hasPlayer(userGameID) && game->getPlayer(userGameID)->hasLost() && currentGame->getPlayer(userGameID)->hasLost(); }
	bool isPaused() { return settings.clientIsPaused(getTime()); }

	double getStartTime() { return completeGame->getStartTime(); }
	int getReferenceID() { return completeGame->getReferenceID(); }

	int getWinCondition() { return settings.gameMode == Mode::MINING ? settings.resourcesToWin : settings.gameMode == Mode::CONQUEST ? settings.outpostsToWin : -1; }
	int getNumTeams() { return settings.number_of_teams; }
	double getSimulationSpeed() { return settings.simulationSpeed; }
	int getWidth() { return settings.width; }
	int getHeight() { return settings.height; }
		
	void bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
	void endBulkAdd();
		
	void addOrder(const String &type, uint32_t ID, int32_t referenceID, bool canceled, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);

	void cancelOrder(uint32_t ID);
};

}

#endif