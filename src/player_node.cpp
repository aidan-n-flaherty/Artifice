#include "player_node.h"
#include "positional_node.h"
#include "../GameLogic/gameClasses/gameObjects/player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

using namespace godot;

void PlayerNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("getUnits"), &PlayerNode::getUnits);
	ClassDB::bind_method(D_METHOD("getCapacity"), &PlayerNode::getCapacity);
	ClassDB::bind_method(D_METHOD("getRating"), &PlayerNode::getRating);
	ClassDB::bind_method(D_METHOD("getName"), &PlayerNode::getName);
	ClassDB::bind_method(D_METHOD("getUserID"), &PlayerNode::getUserID);

}

PlayerNode::PlayerNode(Player* player) : player(player) {
	
}
