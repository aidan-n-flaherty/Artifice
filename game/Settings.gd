extends Control


# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.userChanged.connect(loadUser)
	
	loadUser()
	
func loadUser():
	var user = GameData.getUser()
	if(!user): return
	
	$MarginContainer/VBoxContainer/Username/Username.text = user.username
	$MarginContainer/VBoxContainer/GamesWon/GamesWon.text = str(user.userStats.gamesWon)
	$MarginContainer/VBoxContainer/GamesLost/GamesLost.text = str(user.userStats.gamesLost)
	$MarginContainer/VBoxContainer/Rating/Rating.text = str(user.userStats.rating)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_username_text_changed():
	pass # Replace with function body.
