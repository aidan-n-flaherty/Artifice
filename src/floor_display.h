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
    GameInterface* gameInterface;

    double timeDiff;

    double simulatedDiff;

    double currentTime;

protected:
	static void _bind_methods() {};

public:
    FloorDisplay() {}
    FloorDisplay(GameInterface* gameInterface);
    ~FloorDisplay() {}
		
    void _draw() override;
    
    void setDiff(double timeDiff, double simulatedDiff, double currentTime) {
        this->timeDiff = timeDiff;
        this->simulatedDiff = simulatedDiff;
        this->currentTime = currentTime;
    }
	
	double getDiff() { return timeDiff; }

    double getSimulatedDiff() { return simulatedDiff; }

    double getCurrent() { return currentTime; }
};

}

#endif