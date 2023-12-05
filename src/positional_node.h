#ifndef POSITIONAL_NODE_H
#define POSITIONAL_NODE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/color.hpp>
#include "../GameLogic/gameClasses/gameObjects/positional_object.h"
#include "../GameLogic/gameClasses/order.h"
#include <unordered_set>

namespace godot {

class PositionalNode : public Node3D {
    GDCLASS(PositionalNode, Node3D)

private:
	PositionalObject* obj = nullptr;
	
	double currentTime;

	double timeDiff;

	std::unordered_set<int> selectedSpecialists;

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
		
	void setDiff(double time, double diff) { 
		currentTime = time;
		timeDiff = diff;
	}
	
	double getDiff() { return timeDiff; }
	
	void setReference(PositionalObject* obj, int specialistDisplacement);
	
	int getID() { return obj->getID(); }
	
	int getOwnerID() { return obj->getOwnerID(); }

	int getUnits() { return obj != nullptr ? obj->getUnitsAt(timeDiff) : -1; }
	
	void select(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx);
	
	void selectSpecialist(Camera3D *camera, const Ref<InputEvent> &event, int32_t id);

	virtual void setVisible(bool visible) { set_visible(visible); };

	PositionalObject* getObj() { return obj; }

	void clearSelectedSpecialists() {
		while(!selectedSpecialists.empty()) setSpecialistSelected(*selectedSpecialists.begin(), false);
	}

	void setSpecialistSelected(int specialistID, bool selected);

	bool hasSelectedSpecialist(int specialistID) { return selectedSpecialists.find(specialistID) != selectedSpecialists.end(); }

	void setSelected(bool selected);

	bool isSelected() { return selected; }

	Color getColor();

	bool canUndo() { return obj && obj->getOriginatingOrder() && obj->getOriginatingOrder()->getTimestamp() > currentTime; }

	int getOriginatingOrder() { return canUndo() ? obj->getOriginatingOrder()->getID() : -1; }

	String getOriginatingOrderType() { return canUndo() ? String(obj->getOriginatingOrder()->getType().c_str()) : ""; }
};

}

#endif