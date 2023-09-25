#ifndef VESSEL_NODE_H
#define VESSEL_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include "../GameServer/GameLogic/gameClasses/gameObjects/vessel.h"

namespace godot {

class VesselNode : public PositionalNode {
    GDCLASS(VesselNode, PositionalNode)

private:
	Vessel* vessel;
	
public:
    VesselNode(Vessel* vessel);
	VesselNode() {}
    ~VesselNode() {}

    void _process(double delta) override;
		
	void setReference(Vessel* vessel) {
		PositionalNode::setReference(vessel, 1);
		this->vessel = vessel;

		double angle = atan2(vessel->getTargetPos().getY() - vessel->getPosition().getY(), vessel->getTargetPos().getX() - vessel->getPosition().getX());
		
		for(int i = 0; i < get_child_count(); i++) {
			Node3D* n = cast_to<Node3D>(get_child(i));
			if(n) n->set_rotation(Vector3(0, -angle, 0));
		}
	}

	Vessel* getVessel() { return vessel; }
};

}

#endif