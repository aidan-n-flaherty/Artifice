#ifndef SETTINGSDEFAULT_H
#define SETTINGSDEFAULT_H

#include "../GameLogic/gameClasses/game_settings.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/color.hpp>

namespace godot {

class SettingsDefault : public Node {
    GDCLASS(SettingsDefault, Node)

private:
	static GameSettings defaults;

protected:
	static void _bind_methods();

public:
	SettingsDefault() {}
    ~SettingsDefault() {}
	
	static double getSimulationSpeed(){
		return defaults.simulationSpeed;
	}
	static String getSimulationTimescale(){
		String timescale = "Days";
		return timescale; //default, not currently fully implemented.
	}

	static int getGameMode(){
		return defaults.gameMode;
	}

	static Array getPlayerColors() {
		Array defaultPlayerColors;
		for(auto color : defaults.playerColors) {

			defaultPlayerColors.push_back(Color(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
		}

		return defaultPlayerColors;
	}

	
    /*
	GameSettings::simulationSpeed = 60 * 60; // each hour is a second
    GameSettings::factoryDensity = 0.5;
    GameSettings::resourcesToWin = 200;
    GameSettings::gameMode = CONQUEST;
    GameSettings::eloKValue = 32;
    GameSettings::defaultSonar = 50;
    GameSettings::defaultMaxShield = 20;
    GameSettings::fireRate = 2 * 60 * 60;
    GameSettings::costPerMine = 50;
    GameSettings::outpostsPerPlayer = 5;
    GameSettings::width = 200;
    GameSettings::height = 200;

	
	*/
};

}

#endif