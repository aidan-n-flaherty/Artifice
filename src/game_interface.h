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
	std::shared_ptr<Game> completeGame = nullptr;

	std::shared_ptr<Game> game = nullptr;
	double nextState = 0.0;
	double nextEndState = 0.0;

	std::shared_ptr<Game> currentGame = nullptr;
	double nextCurrentState = 0.0;

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

	double simulationBuffer = 96 * 60 * 60;

	double percent = 1.0;

	double epsilon = 0.00001;

	bool drag = false;

	Point mouse;

protected:
	static void _bind_methods();

public:
    GameInterface() {
		floorDisplay = memnew(FloorDisplay(this));
	}

    ~GameInterface() {}

	void init(int gameID, int userID, int startTime, int playerCap, Dictionary players, Dictionary settingOverrides);

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
		double timeDiff = current - game->getTime();
		return game && game->getPlayer(userGameID) ? game->getPlayer(userGameID)->getHiresAt(timeDiff) : -1;
	}
	
	bool simulatingFuture() { return future; }
	bool willSendWith(SpecialistType type);
	void setSelectedSpecialist(int id);
	void select(int id);
	void setSelected(int id);
	void unselect();
	void setMouse(double x, double y) {
		mouse = Point(game->getSettings(), x, y);
		if(getSelected()) mouse = getSelected()->getPosition().closest(mouse);
		else mouse.constrain();
	}
	const Point& getMouse() { return mouse; }
	void setDrag(bool drag) { this->drag = drag; }
	bool isDragging() { return drag; }; 
	int getSelectedUnits() { return selected != -1 ? selectedUnits : -1; }
	PositionalObject* getSelected() { return getObj(selected); }
	PositionalObject* getObj(int id) { return simulatedGame->getPosObject(id); }
	PositionalNode* getNode(int id);
	
	void shiftToTime(double t);
	void setTime(double t);
	void setTempTime(double t) { tempTime = t; }
	double getTime() { return current + tempTime; }
	double getCurrent() { return current; }

	void setPercent(double percent) { this->percent = percent; }
	double getPercent() { return percent; }

	PositionalNode* getTarget(double x, double y);
	double projectedTime(double x, double y);
	PackedInt32Array getPlayerIDs();
	PackedVector2Array getOutpostPositions();
	PackedInt32Array getShopOptions();
	PackedInt32Array getPromotionOptions(int specialistID);
	Array getPlayers();
	Array getSortedPlayers();
	Array getCurrentSortedPlayers();
	int getScore(int userID);
	Color getColor(int userID);

	int getSpecialistType(int specialistID) { return game->getSpecialist(specialistID)->getType(); };

	PlayerNode* getPlayer(int id);
	String getSpecialistName(int specialistNum);
	String getSpecialistDescription(int specialistNum);
	bool canRelease(int specialistID) {
		return !ownsSpecialist(specialistID) && game->getSpecialist(specialistID)->getContainer() &&
				game->getSpecialist(specialistID)->getContainer()->getOwnerID() == userGameID;
	}
	bool ownsSpecialist(int specialistID) { return game->getSpecialist(specialistID)->getOwnerID() == userGameID; }
	bool ownsObj(int objID) { return game->hasPosObject(objID) && game->getPosObject(objID)->getOwnerID() == userGameID; }

	double getNextArrivalEvent(int vesselID);
	double getNextProductionEvent(int outpostID);
	double getNextBattleEvent(int objID);
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

	bool canHire() { return game->getPlayer(userGameID)->getHiresAt(current) >= 0; }
	bool hasStarted() { return current >= game->getStartTime(); }
	bool hasEnded() { return game->hasEnded() && currentGame->hasEnded(); }

	double getStartTime() { return game->getStartTime(); }
	int getReferenceID() { return game->getReferenceID(); }

	double getSimulationSpeed() { return settings.simulationSpeed; }
	int getWidth() { return settings.width; }
	int getHeight() { return settings.height; }
		
	void bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
	void endBulkAdd();
		
	void addOrder(const String &type, uint32_t ID, int32_t referenceID, double timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);

	void cancelOrder(uint32_t ID);
};

}

#endif