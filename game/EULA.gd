extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_agree_toggled(toggled_on):
	if toggled_on:
		for child in $MarginContainer/VBoxContainer/VBoxContainer.get_children():
			child.show()
	else:
		for child in $MarginContainer/VBoxContainer/VBoxContainer.get_children():
			child.hide()


func _on_start_pressed():
	$MarginContainer/VBoxContainer/VBoxContainer/Start.disabled = true
	
	await GameData.signup()
		
	$MarginContainer/VBoxContainer/VBoxContainer/Start.disabled = false


func _on_rich_text_label_meta_clicked(meta):
	OS.shell_open(str(meta))


func _on_restore_pressed():
	var node = preload("res://AuthCode.tscn").instantiate()
		
	GameData.goto_node(node)
