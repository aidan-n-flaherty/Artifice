extends MarginContainer

var user

var update = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.userChanged.connect(loadSelf)
	
	loadSelf()
	
func loadSelf():
	user = GameData.getSelf()
	if(!user): return
	
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText.text = user.username
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/GridContainer2/Victories.text = str(user.userStats.gamesWon)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/GridContainer2/Defeats.text = str(user.userStats.gamesLost)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/GridContainer2/Rating.text = str(user.userStats.rating)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if update > 0:
		update -= delta
		
		if update <= 0:
			GameData.editSelf(user)
	

func _on_username_text_text_changed(new_text):
	if len(new_text) < 3:
		$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.FIREBRICK
		return
	else:
		$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.WHITE
	user.username = new_text
	
	update = 1

func _exit_tree():
	if update > 0:
		update = -1
		GameData.editSelf(user)
