#ifndef OUTPOST_NODE_H
#define OUTPOST_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include "../GameServer/GameLogic/gameClasses/gameObjects/outpost.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class OutpostNode : public PositionalNode {
    GDCLASS(OutpostNode, PositionalNode)

private:
	Outpost* outpost;

public:
    OutpostNode(Outpost* outpost, time_t referenceTime);
		OutpostNode() {};
    ~OutpostNode() {};

    void _process(double delta) override;
		
		void setReference(Outpost* outpost, time_t referenceTime) {
			PositionalNode::setReference(outpost, referenceTime);
			this->outpost = outpost;
		}
};

}

#endif