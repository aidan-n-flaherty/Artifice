extends MarginContainer

var gameID
# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID: int):
	self.gameID = gameID
	var gameDetails = GameData.getGameDetails(gameID)
	
	$Margin/HBoxContainer/Title.text = gameDetails.gameSettings.lobbyName
	$Margin/HBoxContainer/Players.text = str(gameDetails.gameData.playerCount) + "/" + str(gameDetails.gameSettings.playerCap)
	if gameDetails.gameData.hasPassword:
		$Margin/HBoxContainer/Password.show()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_button_pressed():
	GameData.goto_scene("res://GameDetail.tscn")
