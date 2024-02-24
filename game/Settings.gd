extends MarginContainer

var user

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
	pass

func _on_username_text_text_changed(new_text):
	user.username = new_text
	
	GameData.editSelf(user)
