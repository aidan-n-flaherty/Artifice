extends Area3D


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$Label.rotation = Vector3(deg_to_rad(-90), deg_to_rad(180) - get_rotation().y, 0)
