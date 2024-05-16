extends MarginContainer

var gameID: int

var userID: int

# Called when the node enters the scene tree for the first time.
func _ready():
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().text = "Save"
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().connect("pressed", _on_save_pressed)
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().text = "Start"
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().connect("pressed", _on_start_pressed)
	$MarginContainer/VBoxContainer/GameEditor.setFinalizeText("Ready to start")
	
	GameData.gameChanged.connect(reload)
	GameData.loadUserDetail.connect(viewUser)

func init(gameID):
	self.gameID = gameID
	
	$MarginContainer/VBoxContainer/GameEditor.setEditable(true)
	
	#$Background.material.set_shader_parameter("gradStrength", 0.75)
	
	$Fade.modulate = Color(1.0, 1.0, 1.0)
	
	await GameData.loadGameUsers(gameID)
	
	await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)
	
	$AnimationPlayer.play("fade_from_black")

func reload(gameID: int):
	if self.gameID != gameID:
		return
	
	var details = GameData.getGameDetails(gameID)
	
	if details.gameData.started:
		$AnimationPlayer.play("fade_to_game")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var details = GameData.getGameDetails(gameID)
	
	$MarginContainer/VBoxContainer/GameEditor.setFinalizing(not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap)
	$MarginContainer/VBoxContainer/GameEditor.getSecondaryButton().visible = not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap

func _on_back_button_pressed():
	$AnimationPlayer.play("fade_to_black")

func _on_save_pressed():
	await HTTPManager.putReq("/editMatch", $MarginContainer/VBoxContainer/GameEditor.serialize(), { "gameID": self.gameID })
	
func _on_start_pressed():
	var data = $MarginContainer/VBoxContainer/GameEditor.serialize()
	
	if await HTTPManager.putReq("/editMatch", data, { "gameID": self.gameID, "start": true }):
		GameData.loadGameSettings(self.gameID)
		await $MarginContainer/VBoxContainer/GameEditor.deserialize(self.gameID)

func viewUser(userID: int):
	self.userID = userID
	
	$AnimationPlayer.play("fade_to_user")

func _on_animation_player_animation_finished(anim_name):
	if anim_name == "fade_to_user":
		GameData.viewUserCompletion(userID)
	if anim_name == "fade_to_game":
		GameData.viewGameCompletion($MarginContainer/VBoxContainer/GameEditor.gameID, false)
	if anim_name == "fade_to_current":
		if GameData.exitGameToMenu():
			GameData.currentTab = "res://CurrentGameList.tscn"
			GameData.goto_scene("res://MainMenu.tscn")
	if anim_name == "fade_to_black":
		if GameData.exitGameToMenu():
			GameData.goto_scene("res://MainMenu.tscn")
