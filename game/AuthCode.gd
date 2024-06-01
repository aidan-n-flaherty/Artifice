extends MarginContainer

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$MarginContainer/VBoxContainer/HBoxContainer/Continue.disabled = len($MarginContainer/VBoxContainer/Code.getText()) != 7

func _on_continue_pressed():
	if len($MarginContainer/VBoxContainer/Code.getText()) == 7:
		if not await GameData.getShare($MarginContainer/VBoxContainer/Code.getText()):
			$MarginContainer/VBoxContainer/Label.text = "Invalid code. Try again."

func _on_cancel_pressed():
	GameData.goto_scene("res://EULA.tscn")
