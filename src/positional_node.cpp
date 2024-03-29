#include "vessel_node.h"
#include "../GameLogic/gameClasses/gameObjects/vessel.h"
#include "../GameLogic/gameClasses/game_settings.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <tuple>

using namespace godot;

void PositionalNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("getID"), &PositionalNode::getID);
	ClassDB::bind_method(D_METHOD("getUnits"), &PositionalNode::getUnits);
	ClassDB::bind_method(D_METHOD("getOwnerID"), &PositionalNode::getOwnerID);
	ClassDB::bind_method(D_METHOD("select"), &PositionalNode::select);
	ClassDB::bind_method(D_METHOD("isSelected"), &PositionalNode::isSelected);
	ClassDB::bind_method(D_METHOD("getColor"), &PositionalNode::getColor);
	ClassDB::bind_method(D_METHOD("selectSpecialist"), &PositionalNode::selectSpecialist);
	ClassDB::bind_method(D_METHOD("canUndo"), &PositionalNode::canUndo);
	ClassDB::bind_method(D_METHOD("getOriginatingOrder"), &PositionalNode::getOriginatingOrder);
	ClassDB::bind_method(D_METHOD("getOriginatingOrderType"), &PositionalNode::getOriginatingOrderType);
	ADD_SIGNAL(MethodInfo("selected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("released", PropertyInfo(Variant::INT, "id")));
}

PositionalNode::PositionalNode(const StringName &file, PositionalObject* obj) {
	Ref<PackedScene> mesh = ResourceLoader::get_singleton()->load(file);

	for(int i = -1; i <= 1; i++) for(int j = -1; j <= 1; j++) {
		Node3D* n = cast_to<Node3D>(mesh->instantiate());
		n->set_name(("Mesh" + std::to_string(i) + std::to_string(j)).c_str());
		n->connect("input_event", Callable(this, "select"), Object::CONNECT_DEFERRED);
		n->set_position(Vector3(obj->getSettings()->width * i, 0, obj->getSettings()->height * j));
  		add_child(n);
	}

	set_visible(true);
}

void PositionalNode::_process(double delta) {
	Node::_process(delta);
	
	if(obj == nullptr) return;

	timePassed += delta;
}

void PositionalNode::select(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx) {
	const InputEventMouseButton* e = Object::cast_to<InputEventMouseButton>(event.ptr());
	
	if(e != nullptr && e->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && e->is_pressed()) {
		emit_signal("selected", getID());
	}

	if(e != nullptr && e->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && e->is_released()) {
		emit_signal("released", getID());
	}
}

void PositionalNode::selectSpecialist(Camera3D *camera, const Ref<InputEvent> &event, int32_t id) {
	const InputEventMouseButton* e = Object::cast_to<InputEventMouseButton>(event.ptr());
	
	if(e != nullptr && e->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && e->is_pressed()) {
		emit_signal("selected", getID());
	}

	if(e != nullptr && e->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && e->is_released()) {
		emit_signal("selected", id);
	}
}

void PositionalNode::setReference(PositionalObject* obj, int specialistDisplacement) {
	this->obj = obj;

	for(int i = 0; i < get_child_count(); i++) {
		Node* n = get_child(i)->get_node_or_null("RotationInvariant");
	
		std::list<int> specialistAddition;

		for(Specialist* s : obj->getSpecialists()) {
			bool found = false;

			for(int j = 0; j < n->get_child_count(); j++) {
				Node* child = n->get_child(j);

				if(child->get_name() == StringName(("Specialist" + std::to_string(s->getID())).c_str())) {
					found = true;
					break;
				}
			}

			if(!found) specialistAddition.push_back(s->getID());
		}
		
		std::list<Specialist*> specialists = obj->getSpecialists();

		for(int j = 0; j < n->get_child_count(); j++) {
			Node* child = n->get_child(j);

			if(!std::any_of(specialists.begin(), specialists.end(), [&child](auto s) {
				return child->get_name() == StringName(("Specialist" + std::to_string(s->getID())).c_str());
			}) && child->get_name().contains("Specialist")) {
				child->queue_free();
				n->remove_child(child);
				j--;
			}
		}
		
		std::list<Node3D*> currentSpecialists;

		for(Specialist* sp : specialists) {
			bool contains = false;
			Node* specialist;

			for(int j = 0; j < n->get_child_count(); j++) {
				Node* child = n->get_child(j);

				if(child->get_name() == StringName(("Specialist" + std::to_string(sp->getID())).c_str())) {
					contains = true;
					specialist = child;
					currentSpecialists.push_back(cast_to<Node3D>(child));

					break;
				}
			}

			if(!contains) {
				Ref<PackedScene> sMesh = ResourceLoader::get_singleton()->load("res://SpecialistMesh.tscn");
				Node3D* s = cast_to<Node3D>(sMesh->instantiate());
				s->set_name(("Specialist" + std::to_string(sp->getID())).c_str());
				s->connect("custom_input_event", Callable(this, "selectSpecialist"), Object::CONNECT_DEFERRED);

				specialist = s;

				n->add_child(s);

				currentSpecialists.push_back(s);
			}

			Ref<Texture2D> img = ResourceLoader::get_singleton()->load((std::string("res://resources/specialistIcons/") + sp->typeAsString() + ".png").c_str());
			TextureRect* texture = cast_to<TextureRect>(specialist->get_node_or_null(NodePath("SubViewport/Control/Texture")));	
			if(texture) texture->set_texture(img);

			CanvasItem* item = cast_to<CanvasItem>(specialist->get_node_or_null("SubViewport/Control"));
			ShaderMaterial* mat1 = cast_to<ShaderMaterial>(item->get_material().ptr());

			MeshInstance3D* mesh = cast_to<MeshInstance3D>(specialist->get_node_or_null("Cylinder"));
			ShaderMaterial* mat2 = cast_to<ShaderMaterial>(mesh->get_surface_override_material(0).ptr());

			if(selectedSpecialists.find(sp->getID()) != selectedSpecialists.end()) {
				mat1->set_shader_parameter("inverted", true);
				mat2->set_shader_parameter("inverted", true);
			} else {
				mat1->set_shader_parameter("inverted", false);
				mat2->set_shader_parameter("inverted", false);
			}

			if(sp->getContainer()->getOwnerID() != sp->getOwnerID()) {
				mat1->set_shader_parameter("translucent", true);
				mat2->set_shader_parameter("translucent", true);
			} else {
				mat1->set_shader_parameter("translucent", false);
				mat2->set_shader_parameter("translucent", false);
			}
		}

		int offset = specialistDisplacement;

		for(Node3D* child : currentSpecialists) {
			int diagonalDisplacement = 4.1 * (offset / 3) + sqrt(4.5) * abs((offset + 1) % 3 - 1);
			child->set_position(Vector3(3.0 * ((offset + 1) % 3 - 1), 40 + diagonalDisplacement, -40 + diagonalDisplacement));
			offset++;
		}
	}
}

void PositionalNode::setSpecialistSelected(int specialistID, bool selected) {
	if(selected) selectedSpecialists.insert(specialistID);
	else selectedSpecialists.erase(specialistID);

	for(int i = 0; i < get_child_count(); i++) {
		Node* n = get_child(i)->get_node_or_null("RotationInvariant");

		for(int j = 0; j < n->get_child_count(); j++) {
			Node* child = n->get_child(j);

			if(child->get_name() == StringName(("Specialist" + std::to_string(specialistID)).c_str())) {
				CanvasItem* item = cast_to<CanvasItem>(child->get_node_or_null(NodePath("SubViewport/Control")));
				ShaderMaterial* mat1 = cast_to<ShaderMaterial>(item->get_material().ptr());

				MeshInstance3D* mesh = cast_to<MeshInstance3D>(child->get_node_or_null(NodePath("Cylinder")));
				ShaderMaterial* mat2 = cast_to<ShaderMaterial>(mesh->get_surface_override_material(0).ptr());

				if(selected) {
					mat1->set_shader_parameter("inverted", true);
					mat2->set_shader_parameter("inverted", true);
				} else {
					mat1->set_shader_parameter("inverted", false);
					mat2->set_shader_parameter("inverted", false);
				}
			}
		}
	}
}

void PositionalNode::setSelected(bool selected) {
	if(this->selected == selected) return;

	this->selected = selected;

	/*if(selected) {
		for(int i = 0; i < get_child_count(); i++) {
			Node* n = get_child(i);

			Ref<PackedScene> sMesh = ResourceLoader::get_singleton()->load("res://SelectionCircle.tscn");
			Node3D* s = cast_to<Node3D>(sMesh->instantiate());
			s->set_name("SelectionCircle");
			s->set_position(Vector3(0, 0, 0));
			n->add_child(s);
		}
	} else {
		for(int i = 0; i < get_child_count(); i++) {
			Node* n = get_child(i);
			
			for(int j = 0; j < n->get_child_count(); j++) {
				Node* child = n->get_child(j);

				if(child->get_name().contains("SelectionCircle")) {
					child->queue_free();
					n->remove_child(child);
					j--;
				}
			}
		}
	}*/
}

Color PositionalNode::getColor() {
	std::tuple<double, double, double> color = obj->hasOwner() ? obj->getSettings()->playerColors[obj->getOwnerID() % obj->getSettings()->playerColors.size()] : std::make_tuple(0.5, 0.5, 0.5);

	return Color(std::get<0>(color), std::get<1>(color), std::get<2>(color));
}