extends Node3D

var dark = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func setID(id: int):
	$MeshInstance3D.get_surface_override_material(1).set_shader_parameter("id", id)
	
	var rot = PI * sin(id)
	$MeshInstance3D.rotation.y = rot
	$MeshInstance3D.get_surface_override_material(2).set_shader_parameter("rotation", rot)

func setDark(dark: bool):
	$MeshInstance3D.get_surface_override_material(0).albedo_color = Color(0.0, 0.0, 0.0, 0.11) if dark else Color(1.0, 1.0, 1.0, 0.11)
	$MeshInstance3D.get_surface_override_material(1).set_shader_parameter("dark", dark)
	$MeshInstance3D.get_surface_override_material(2).set_shader_parameter("dark", dark)

func showLights(lights: bool):
	$MeshInstance3D.get_surface_override_material(4).transparency = 0 if lights else 1

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
