extends MarginContainer

var gameID: int

var inGame = false

var userID: int

# Called when the node enters the scene tree for the first time.
func _ready():
	$MarginContainer/VBoxContainer/GameEditor.setActivatable(true)
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().connect("pressed", _on_button_pressed)
		
	GameData.gameChanged.connect(reload)
	GameData.loadUserDetail.connect(viewUser)

func init(gameID):
	self.gameID = gameID
	
	$MarginContainer/VBoxContainer/GameEditor.setEditable(false)
	
	#$Background.material.set_shader_parameter("gradStrength", 0.75)
	
	$Fade.modulate = Color(1.0, 1.0, 1.0)
	
	await GameData.loadGameUsers(gameID)
	
	await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)
	
	reload(gameID)
	
	$AnimationPlayer.play("fade_from_black")

func reload(gameID: int):
	if gameID != self.gameID:
		return
	
	inGame = false
	
	var users = await GameData.getGameUsers(self.gameID)
	
	for user in users.values():
		if int(user.id) == GameData.getSelfID():
			inGame = true
			break
	
	if inGame:
		$MarginContainer/VBoxContainer/GameEditor.getActivationButton().text = "Leave"
	else:
		$MarginContainer/VBoxContainer/GameEditor.getActivationButton().text = "Join"
	
	var details = GameData.getGameDetails(gameID)
	
	if int(details.gameData.startTime) < Time.get_unix_time_from_system() + (2 * 365 * 24 * 60 * 60):
		$AnimationPlayer.play("fade_to_game")
	else:
		await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var details = GameData.getGameDetails(gameID)
	
	$MarginContainer/VBoxContainer/GameEditor.setFinalizing(not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap)

func _on_back_button_pressed():
	$AnimationPlayer.play("fade_to_black")

func _on_button_pressed():
	if not inGame:
		await GameData.joinGame(self.gameID, $MarginContainer/VBoxContainer/GameEditor/MarginContainer/VBoxContainer/Passworded/PasswordText.text)
	else:
		if await GameData.leaveGame(self.gameID):
			$AnimationPlayer.play("fade_to_current")

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
