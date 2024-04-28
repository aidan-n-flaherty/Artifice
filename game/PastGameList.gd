extends GameList
	
func initList():
	gameIDs = GameData.getPastGames()
	
func generateButton(id):
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id, true, true)
	return button


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
