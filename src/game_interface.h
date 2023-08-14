#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <godot_cpp/classes/node3d.hpp>
#include <list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include "../GameServer/GameLogic/gameClasses/game.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameServer/GameLogic/gameClasses/game_settings.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include "floor_display.h"

namespace godot {

class GameInterface : public Node3D {
    GDCLASS(GameInterface, Node3D)

private:
	std::shared_ptr<Game> completeGame;
	std::shared_ptr<Game> game;
	
	std::unordered_map<int, VesselNode*> vessels;
	std::unordered_map<int, OutpostNode*> outposts;

	FloorDisplay* floorDisplay;
	
	int userID, userGameID;

	double nextState;
	
	int selectedObj = -1;
	
	std::unordered_set<int> selectedSpecialists;
	
	double current;
	
	bool future = true;

	double percent = 1.0;

protected:
	static void _bind_methods();

public:
    GameInterface() {
		floorDisplay = memnew(FloorDisplay(this));
	}

    ~GameInterface() {}

	void init(int userID);
		
    void _process(double delta) override;
		
	void update();

	FloorDisplay* getFloorDisplay() { return floorDisplay; }

	std::shared_ptr<Game> getCompleteGame() { return completeGame; }
	std::shared_ptr<Game> getGame() { return game; }

	int getUserGameID() { return userGameID; }
		
	void select(int id);
	void setSelected(int id);
	void unselect() { setSelected(-1); }
	int getSelected() { return selectedObj; }
		
	void setTime(double t);
	double getTime() { return current; }

	void setPercent(double percent) { this->percent = percent; }
	double getPercent() { return percent; }

	int getWidth() { return GameSettings::width; }
	int getHeight() { return GameSettings::height; }
		
	void bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
	void endBulkAdd();
		
	void addOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
};

}

#endif