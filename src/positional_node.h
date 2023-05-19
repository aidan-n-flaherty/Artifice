#ifndef POSITIONAL_NODE_H
#define POSITIONAL_NODE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include "../GameServer/GameLogic/gameClasses/gameObjects/positional_object.h"

namespace godot {

class PositionalNode : public Node3D {
    GDCLASS(PositionalNode, Node3D)

private:
	PositionalObject* obj;
	
	time_t referenceTime;

protected:
	static void _bind_methods();
	
public:
    PositionalNode(const StringName &file, PositionalObject* obj, time_t referenceTime);
		PositionalNode() {};
    ~PositionalNode() {};

    virtual void _process(double delta) override;
		
		virtual void setReference(PositionalObject* obj, time_t referenceTime) {
			this->obj = obj;
			this->referenceTime = referenceTime;
		}
		
		int getID() { return obj->getID(); }
		
		int getOwnerID() { return obj->getOwnerID(); }
		
		time_t getReferenceTime() { return referenceTime; }
		
		void select(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx);
};

}

#endif