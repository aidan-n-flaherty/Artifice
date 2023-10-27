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
	ClassDB::bind_method(D_METHOD("getID"), &PlayerNode::getID);
	ClassDB::bind_method(D_METHOD("getUserID"), &PlayerNode::getUserID);
	ClassDB::bind_method(D_METHOD("getColor"), &PlayerNode::getColor);
	ClassDB::bind_method(D_METHOD("getResources"), &PlayerNode::getResources);
	ClassDB::bind_method(D_METHOD("getOutposts"), &PlayerNode::getOutposts);
	ClassDB::bind_method(D_METHOD("getVessels"), &PlayerNode::getVessels);
	ClassDB::bind_method(D_METHOD("getFactories"), &PlayerNode::getFactories);
	ClassDB::bind_method(D_METHOD("getMines"), &PlayerNode::getMines);
	ClassDB::bind_method(D_METHOD("getGenerators"), &PlayerNode::getGenerators);
}

PlayerNode::PlayerNode(Player* player) : player(player) {
	
}
