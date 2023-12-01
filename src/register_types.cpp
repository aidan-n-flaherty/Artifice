#include "register_types.h"

#include "gdexample.h"
#include "game_interface.h"
#include "vessel_node.h"
#include "outpost_node.h"
#include "floor_display.h"
#include "settingsDefault.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_gamelogic_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<GDExample>();
    ClassDB::register_class<GameInterface>();
    ClassDB::register_class<FloorDisplay>();
    ClassDB::register_class<PositionalNode>();
    ClassDB::register_class<VesselNode>();
    ClassDB::register_class<OutpostNode>();
    ClassDB::register_class<PlayerNode>();
	ClassDB::register_class<SettingsDefault>();
}

void uninitialize_gamelogic_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT gamelogic_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_gamelogic_module);
    init_obj.register_terminator(uninitialize_gamelogic_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}