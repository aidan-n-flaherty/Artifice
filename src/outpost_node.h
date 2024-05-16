#ifndef OUTPOST_NODE_H
#define OUTPOST_NODE_H

#include "positional_node.h"
#include <godot_cpp/classes/node3d.hpp>
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameLogic/gameClasses/gameObjects/player.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class OutpostNode : public PositionalNode {
    GDCLASS(OutpostNode, PositionalNode)

private:
	Outpost* outpost = nullptr;

protected:
	static void _bind_methods();

public:
    OutpostNode(Outpost* outpost);
	OutpostNode() {}
    ~OutpostNode() {}

    void _process(double delta) override;
		
	void setReference(Outpost* outpost) {
		PositionalNode::setReference(outpost, 1);
		this->outpost = outpost;
	}

	Outpost* getOutpost(){ return outpost; }

	int getProductionAmount() { return outpost->getProductionAmount(); }

	int getShield() { return outpost ? outpost->getShieldAt(getDiff()) : -1; }

	int getMaxShield() { return outpost ? outpost->getMaxShield() : -1; }

	int getMineCost() { return outpost && outpost->getOwner() ? outpost->getOwner()->getMineCost() : -1; }

	bool canMine() { return outpost->getOwner() && outpost->getUnits() >= outpost->getOwner()->getMineCost() && outpost->getType() != OutpostType::MINE; }

	bool isFactory() { return outpost->getType() == OutpostType::FACTORY; }

	bool isGenerator() { return outpost->getType() == OutpostType::GENERATOR; }

	bool isMine() { return outpost->getType() == OutpostType::MINE; }

	bool isBroken() { return outpost->getType() == OutpostType::BROKEN; }

	String getName();
};

}

#endif