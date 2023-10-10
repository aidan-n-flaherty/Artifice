extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(username, points):
	$MarginContainer/HBoxContainer/Username.text = str(username)
	$MarginContainer/HBoxContainer/Points.text = str(points)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
