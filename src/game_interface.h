#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <godot_cpp/classes/node3d.hpp>
#include <list>
#include <unordered_map>
#include "../GameServer/GameLogic/gameClasses/game.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include "vessel_node.h"
#include "outpost_node.h"

namespace godot {

class GameInterface : public Node3D {
    GDCLASS(GameInterface, Node3D)

private:
	std::shared_ptr<Game> completeGame;
	std::shared_ptr<Game> game;
	
	std::unordered_map<int, VesselNode*> vessels;
	std::unordered_map<int, OutpostNode*> outposts;
	
	time_t nextState;
	
	int selectedObj = -1;
	
	std::list<int> selectedSpecialists;
	
	time_t current;
	
	bool future = true;

protected:
	static void _bind_methods();

public:
    GameInterface();
    ~GameInterface() {};

		void init();
		
    void _process(double delta) override;
		
		void update();
		
		void select(int id);
		
		void setTime(uint32_t t);
		
		uint32_t getTime() { return current; };
		
		void bulkAddOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
		
		void endBulkAdd();
		
		void addOrder(const String &type, uint32_t ID, int32_t referenceID, uint32_t timestamp, uint32_t senderID, PackedInt32Array arguments, uint32_t argCount);
};

}

#endif