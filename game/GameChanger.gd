extends MarginContainer

var gameID: int

var userID: int

# Called when the node enters the scene tree for the first time.
func _ready():
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().text = "Save"
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().connect("pressed", _on_save_pressed)
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().show()
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().text = "Start"
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().connect("pressed", _on_start_pressed)
	$MarginContainer/VBoxContainer/GameEditor.setFinalizeText("Ready to start")
	
	GameData.gameChanged.connect(reload)

func init(gameID):
	self.gameID = gameID
	
	$MarginContainer/VBoxContainer/GameEditor.setEditable(true)
	
	#$Background.material.set_shader_parameter("gradStrength", 0.75)
	
	await GameData.loadGameUsers(gameID)
	
	await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)

func reload(gameID: int):
	if self.gameID != gameID:
		return

	var details = GameData.getGameDetails(gameID)
	
	if details.gameData.started:
		GameData.viewGame(self.gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var details = GameData.getGameDetails(gameID)
	
	$MarginContainer/VBoxContainer/GameEditor.setFinalizing(not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap)
	
	if not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap:
		$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().text = "Start"
	else:
		$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().text = "Leave"

func _on_back_button_pressed():
	GameData.previous()

func _on_save_pressed():
	await HTTPManager.putReq("/editMatch", $MarginContainer/VBoxContainer/GameEditor.serialize(), { "gameID": self.gameID })
	
func _on_start_pressed():
	var details = GameData.getGameDetails(gameID)
	
	if not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap:
		var data = $MarginContainer/VBoxContainer/GameEditor.serialize()
		
		if await HTTPManager.putReq("/editMatch", data, { "gameID": self.gameID, "start": true }):
			GameData.loadGameSettings(self.gameID)
			await $MarginContainer/VBoxContainer/GameEditor.deserialize(self.gameID)
	else:
		if await GameData.leaveGame(self.gameID):
			GameData.gotoCurrent()
