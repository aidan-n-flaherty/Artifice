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
    VesselNode(Vessel* vessel, time_t referenceTime);
		VesselNode() {};
    ~VesselNode() {};

    void _process(double delta) override;
		
		void setReference(Vessel* vessel, time_t referenceTime) {
			PositionalNode::setReference(vessel, referenceTime);
			this->vessel = vessel;
		}
};

}

#endif