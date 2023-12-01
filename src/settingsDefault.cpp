#include "settingsDefault.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

using namespace godot;

void SettingsDefault::_bind_methods() {
	ClassDB::bind_static_method("SettingsDefault",D_METHOD("getSimulationSpeed"), &SettingsDefault::getSimulationSpeed);
	ClassDB::bind_static_method("SettingsDefault",D_METHOD("getSimulationTimescale"), &SettingsDefault::getSimulationTimescale);
	ClassDB::bind_static_method("SettingsDefault",D_METHOD("getGameMode"), &SettingsDefault::getGameMode);
	ClassDB::bind_static_method("SettingsDefault",D_METHOD("getPlayerColors"), &SettingsDefault::getPlayerColors);

}

GameSettings SettingsDefault::defaults = GameSettings();

