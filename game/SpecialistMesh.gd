extends Node3D

signal input_event(camera: Camera2D, event: InputEvent, id: int)

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$Area2D.position = get_viewport().get_camera_3d().unproject_position(global_transform.origin)


func _on_area_2d_input_event(viewport, event, shape_idx):
	input_event.emit(get_viewport().get_camera_2d(), event, name.replace("Specialist", "").to_int())
