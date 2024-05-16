extends GameList
	
func initList():
	gameIDs = GameData.getOngoingGames()
	
	gameIDs.sort_custom(func(a, b):
		var gameDetailsA = GameData.getGameDetails(a)
		return gameDetailsA.gameData.hasNotifications or gameDetailsA.gameData.hasChatNotifications)
	
	if len(gameIDs) > 0:
		$NoGames.hide()
	else:
		$NoGames.show()

func generateButton(id):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id, true)
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

