#include "vessel_node.h"
#include "positional_node.h"
#include "../GameLogic/gameClasses/gameObjects/vessel.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

using namespace godot;


void VesselNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("isGift"), &VesselNode::isGift);
}

VesselNode::VesselNode(Vessel* vessel) : PositionalNode("res://SubMesh.tscn", vessel), vessel(vessel) {
	double angle = atan2(vessel->getTargetPos().getY() - vessel->getPosition().getY(), vessel->getTargetPos().getX() - vessel->getPosition().getX());
	
	for(int i = 0; i < get_child_count(); i++) {
		Node3D* n = cast_to<Node3D>(get_child(i));
		if(n) n->set_rotation(Vector3(0, -angle, 0));
	}
}

void VesselNode::_process(double delta) {	
	PositionalNode::_process(delta);
		
	if(vessel == nullptr) return;
		
    set_position(Vector3(vessel->getPositionAt(getDiff()).getX(), 25, vessel->getPositionAt(getDiff()).getY()));
}