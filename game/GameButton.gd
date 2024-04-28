extends MarginContainer

var gameID

var gameDetails

var alreadyJoined

var past

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID: int, alreadyJoined: bool, past=false):
	GameData.gamesChanged.connect(update)
	
	self.alreadyJoined = alreadyJoined
	self.past = past
	
	self.gameID = gameID
	update(self.gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func update(gameID):
	gameDetails = GameData.getGameDetails(gameID)
	
	$Margin/HBoxContainer/Title.text = gameDetails.gameSettings.lobbyName
	$Margin/HBoxContainer/Players.text = str(gameDetails.gameData.playerCount) + "/" + str(gameDetails.gameSettings.playerCap)
	if gameDetails.gameData.hasPassword:
		$Margin/HBoxContainer/Password.show()

func _on_button_pressed():
	if alreadyJoined:
		GameData.viewGame(gameID, past)
	else:
		GameData.viewGameDetail(gameID)
