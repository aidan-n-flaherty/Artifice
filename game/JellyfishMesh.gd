extends Node3D

var dark = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func setDark(dark: bool):
	$MeshInstance3D.get_surface_override_material(1).set_shader_parameter("dark", dark)
	$MeshInstance3D.get_surface_override_material(2).set_shader_parameter("dark", dark)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
