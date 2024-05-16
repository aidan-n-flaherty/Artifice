extends GameList

var userID = null

func initUser(userID):
	self.userID = userID

func initList():
	if not userID:
		gameIDs = await GameData.getPastGames(GameData.getSelfID())
	else:
		gameIDs = await GameData.getPastGames(userID)
	
func generateButton(id):
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id, true, true)
	return button


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
