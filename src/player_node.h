#ifndef PLAYER_NODE_H
#define PLAYER_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include "../GameLogic/gameClasses/gameObjects/player.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class PlayerNode : public Node {
    GDCLASS(PlayerNode, Node)

private:
	Player* player;

protected:
	static void _bind_methods();

public:
    PlayerNode(Player* player);
	PlayerNode() {}
    ~PlayerNode() {}
		
	Player* getPlayer(){ return player; }
	
};

}

#endif