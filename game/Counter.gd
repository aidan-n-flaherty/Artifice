extends Area3D


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setValue(val):
	$SubViewport/Label.text = val

func setColor(color):
	$MeshInstance3D.material_override.albedo_color = color

func setSelection(selected):
	print(selected)
	$SubViewport/Label.modulate = Color(0, 0, 0) if selected else Color(1, 1, 1)
	$MeshInstance3D2.material_override.albedo_color = Color(1, 1, 1) if selected else Color(0, 0, 0)
