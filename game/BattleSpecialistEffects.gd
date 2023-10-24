extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(p1, phaseName, battleMessages):
	$MarginContainer/VBoxContainer/Title.text = phaseName
	for message in battleMessages:
		var displayMessage
		displayMessage = preload("res://ForecastMessage.tscn").instantiate()
		displayMessage.init(p1, message[0], message[1])
		$MarginContainer/VBoxContainer/VBoxContainer.add_child(displayMessage)
		

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
