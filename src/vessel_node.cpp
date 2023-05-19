#include "vessel_node.h"
#include "positional_node.h"
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"
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

VesselNode::VesselNode(Vessel* vessel, time_t referenceTime) : PositionalNode("res://AirshipMesh.tscn", vessel, referenceTime), vessel(vessel) {
	double angle = atan2(vessel->getTargetPos().getY() - vessel->getPosition().getY(), vessel->getTargetPos().getX() - vessel->getPosition().getX());
	
	rotate_y(-angle);
}

void VesselNode::_process(double delta) {	
		PositionalNode::_process(delta);
		
		if(vessel == nullptr) return;
		
		time_t current;
		time(&current);
		
		auto now = std::chrono::system_clock::now();
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto fraction = std::chrono::duration_cast<std::chrono::milliseconds>(now - seconds).count()/1000.0;
		time_t cnow = std::chrono::system_clock::to_time_t(now);

		double timeDiff = difftime(cnow, getReferenceTime()) + fraction;
		
    set_position(Vector3(vessel->getPositionAt(timeDiff).getX(), 10, vessel->getPositionAt(timeDiff).getY()));
}