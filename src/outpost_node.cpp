#include "outpost_node.h"
#include "positional_node.h"
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
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
	ClassDB::bind_method(D_METHOD("getMaxShield"), &OutpostNode::getMaxShield);
	ClassDB::bind_method(D_METHOD("getProductionAmount"), &OutpostNode::getProductionAmount);
	ClassDB::bind_method(D_METHOD("canProduce"), &OutpostNode::canProduce);
	ClassDB::bind_method(D_METHOD("canMine"), &OutpostNode::canMine);
	ClassDB::bind_method(D_METHOD("getMineCost"), &OutpostNode::getMineCost);
	ClassDB::bind_method(D_METHOD("isMine"), &OutpostNode::isMine);
	ClassDB::bind_method(D_METHOD("isFactory"), &OutpostNode::isFactory);
	ClassDB::bind_method(D_METHOD("isGenerator"), &OutpostNode::isGenerator);
}

OutpostNode::OutpostNode(Outpost* outpost) : PositionalNode("res://CityMesh.tscn", outpost), outpost(outpost) {
	
}

void OutpostNode::_process(double delta) {
	PositionalNode::_process(delta);
	if(outpost == nullptr) return;
		
    set_position(Vector3(outpost->getPositionAt(getDiff()).getX(), 0, outpost->getPositionAt(getDiff()).getY()));
}