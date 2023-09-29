#ifndef OUTPOST_NODE_H
#define OUTPOST_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameLogic/gameClasses/gameObjects/player.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class OutpostNode : public PositionalNode {
    GDCLASS(OutpostNode, PositionalNode)

private:
	Outpost* outpost;

protected:
	static void _bind_methods();

public:
    OutpostNode(Outpost* outpost);
	OutpostNode() {}
    ~OutpostNode() {}

    void _process(double delta) override;
		
	void setReference(Outpost* outpost) {
		PositionalNode::setReference(outpost, 2);
		this->outpost = outpost;
	}

	Outpost* getOutpost(){ return outpost; }

	int getShield() { return outpost != nullptr ? outpost->getShieldAt(getDiff()) : -1; }

	bool canProduce() { return outpost->getType() == OutpostType::FACTORY; }

	bool canMine() { return outpost->getOwner() && outpost->getUnits() >= outpost->getOwner()->getMineCost() && outpost->getType() != OutpostType::MINE; }
};

}

#endif