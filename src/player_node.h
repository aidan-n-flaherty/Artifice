#ifndef PLAYER_NODE_H
#define PLAYER_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include "../GameLogic/gameClasses/gameObjects/player.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class PlayerNode : public Node {
    GDCLASS(PlayerNode, Node)

private:
	Player* player;
    double currentTime;
	double timeDiff;

protected:
	static void _bind_methods();

public:
    PlayerNode(Player* player);
	PlayerNode() {}
    ~PlayerNode() {}
		
	Player* getPlayer(){ return player; }
	
    void setReference(Player* player) { this->player = player;}

    void setDiff(double time, double diff) { 
		currentTime = time;
		timeDiff = diff;
	}
	
    int getUnits() { return player->getUnitsAt(timeDiff); }

    int getCapacity() { return player->getCapacity(); }

	int getRating() { return player->getRating(); }

	String getName() { return String(player->getName().c_str()); }

	int getUserID() { return player->getUserID(); }

	double getDiff() { return timeDiff; }
};

}

#endif