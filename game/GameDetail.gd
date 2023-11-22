extends MarginContainer

var gameID

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID):
	self.gameID = gameID
	
	var details = GameData.getGameDetails(gameID)
	
	$MarginContainer/VBoxContainer/Title.text = details.gameSettings.lobbyName

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_back_button_pressed():
	GameData.goto_scene("res://MainMenu.tscn")


func _on_button_pressed():
	if await GameData.joinGame(gameID):
		GameData.goto_scene("res://MainMenu.tscn")
