extends Control


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
func init(username, color):
	$HBoxContainer/Name.text = str(username)

	$HBoxContainer/PlayerIcon.modulate = color
	var slightColor = color
	slightColor.a = 0.25
	$Panel.modulate = slightColor
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
