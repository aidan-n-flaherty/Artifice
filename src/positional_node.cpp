#include "vessel_node.h"
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

void PositionalNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("getID"), &PositionalNode::getID);
	ClassDB::bind_method(D_METHOD("select"), &PositionalNode::select);
	ADD_SIGNAL(MethodInfo("selected", PropertyInfo(Variant::INT, "id")));
}

PositionalNode::PositionalNode(const StringName &file, PositionalObject* obj, time_t referenceTime) : obj(obj), referenceTime(referenceTime) {
	Ref<PackedScene> mesh = ResourceLoader::get_singleton()->load(file);

	Node* n = mesh->instantiate();
	n->connect("input_event", Callable(this, "select"), Object::CONNECT_DEFERRED);
  add_child(n);
}

void PositionalNode::_process(double delta) {	
		Node::_process(delta);
}

void PositionalNode::select(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx) {
	const InputEventMouseButton* e = Object::cast_to<InputEventMouseButton>(event.ptr());
	
	if(e != nullptr && e->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && e->is_pressed()) {
		emit_signal("selected", getID());
	}
}