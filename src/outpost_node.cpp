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

OutpostNode::OutpostNode(Outpost* outpost, time_t referenceTime) : PositionalNode("res://CityMesh.tscn", outpost, referenceTime), outpost(outpost) {
	
}

void OutpostNode::_process(double delta) {
		PositionalNode::_process(delta);
		if(outpost == nullptr) return;
	
		time_t current;
		time(&current);
		
		double timeDiff = difftime(current, getReferenceTime());
    set_position(Vector3(outpost->getPositionAt(timeDiff).getX(), 0, outpost->getPositionAt(timeDiff).getY()));
}