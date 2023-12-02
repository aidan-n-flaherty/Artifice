extends Control


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
func init(username, color):
	$MarginContainer/HBoxContainer/Name.text = str(username)

	$MarginContainer/HBoxContainer/PlayerIcon.modulate = color

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
