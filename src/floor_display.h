#ifndef FLOOR_DISPLAY_H
#define FLOOR_DISPLAY_H

#include <godot_cpp/classes/control.hpp>
#include <list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

namespace godot {

class GameInterface;

class FloorDisplay : public Control {
    GDCLASS(FloorDisplay, Control)

private:
    GameInterface* game;

    double timeDiff;

protected:
	static void _bind_methods() {};

public:
    FloorDisplay() {}
    FloorDisplay(GameInterface* game);
    ~FloorDisplay() {}
		
    void _draw() override;
    
    void setDiff(double diff) { timeDiff = diff; }
	
	double getDiff() { return timeDiff; }
};

}

#endif