#ifndef POSITIONAL_NODE_H
#define POSITIONAL_NODE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include "../GameServer/GameLogic/gameClasses/gameObjects/positional_object.h"

namespace godot {

class PositionalNode : public Node3D {
    GDCLASS(PositionalNode, Node3D)

private:
	PositionalObject* obj;
	
	double timeDiff;

	bool selected;

	double timePassed;

protected:
	static void _bind_methods();
	
public:
    PositionalNode(const StringName &file, PositionalObject* obj);
	PositionalNode() {}
    ~PositionalNode() {}

    virtual void _process(double delta) override;

	double getTimePassed() { return timePassed; }
		
	void setDiff(double diff) { timeDiff = diff; }
	
	double getDiff() { return timeDiff; }
	
	virtual void setReference(PositionalObject* obj);
	
	int getID() { return obj->getID(); }
	
	int getOwnerID() { return obj->getOwnerID(); }

	int getUnits() { return obj != nullptr ? obj->getUnitsAt(timeDiff) : 0; }
	
	void select(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx);
	
	void selectSpecialist(Camera2D *camera, const Ref<InputEvent> &event, int32_t id);

	PositionalObject* getObj() { return obj; }

	void setSelected(bool selected);

	bool isSelected() { return selected; }
};

}

#endif