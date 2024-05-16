extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_agree_toggled(toggled_on):
	if toggled_on:
		$VBoxContainer/VBoxContainer/Spacer.show()
		$VBoxContainer/VBoxContainer/Start.show()
	else:
		$VBoxContainer/VBoxContainer/Spacer.hide()
		$VBoxContainer/VBoxContainer/Start.hide()


func _on_start_pressed():
	GameData.goto_scene("res://Signup.tscn")


func _on_rich_text_label_meta_clicked(meta):
	OS.shell_open(str(meta))
