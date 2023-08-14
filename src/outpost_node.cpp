#include "outpost_node.h"
#include "positional_node.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

using namespace godot;

void OutpostNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("getShield"), &OutpostNode::getShield);
}

OutpostNode::OutpostNode(Outpost* outpost) : PositionalNode("res://CityMesh.tscn", outpost), outpost(outpost) {
	
}

void OutpostNode::_process(double delta) {
	PositionalNode::_process(delta);
	if(outpost == nullptr) return;
		
    set_position(Vector3(outpost->getPositionAt(getDiff()).getX(), 0, outpost->getPositionAt(getDiff()).getY()));
}