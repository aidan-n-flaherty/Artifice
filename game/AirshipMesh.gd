extends Area3D

var mirror

# Called when the node enters the scene tree for the first time.
func _ready():
	#mirror = preload("res://resources/airship/alternate_airship.blend").instantiate()
	#add_child(mirror)
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	#mirror.position = Vector3(0.305, -2.0 * global_position.y, 0)
	#mirror.rotation.y = deg_to_rad(-90)
	#mirror.scale = Vector3(0.9, -0.9, 0.9)
	$Label.rotation = Vector3(deg_to_rad(-90), deg_to_rad(180) - get_rotation().y, 0)