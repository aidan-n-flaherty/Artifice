extends MarginContainer

var gameID: int

var inGame = false

var userID: int

# Called when the node enters the scene tree for the first time.
func _ready():
	$MarginContainer/VBoxContainer/GameEditor.setActivatable(true)
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().connect("pressed", _on_button_pressed)
		
	GameData.gameChanged.connect(reload)

func init(gameID):
	self.gameID = gameID
	
	$MarginContainer/VBoxContainer/GameEditor.setEditable(false)
	
	#$Background.material.set_shader_parameter("gradStrength", 0.75)
	
	await GameData.loadGameUsers(gameID)
	
	await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)
	
	reload(gameID)

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
		GameData.viewGame(gameID)
	else:
		await $MarginContainer/VBoxContainer/GameEditor.deserialize(gameID)
	
	if int(details.gameData.hostID) == GameData.getSelfID():
		var gameChanger = preload("res://GameChanger.tscn").instantiate()
		gameChanger.init(gameID)
		
		GameData.goto_node(gameChanger)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var details = GameData.getGameDetails(gameID)
	
	$MarginContainer/VBoxContainer/GameEditor.setFinalizing(not details.gameData.started and details.gameData.playerCount == details.gameSettings.playerCap)

func _on_back_button_pressed():
	GameData.previous()

func _on_button_pressed():
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().disabled = true
	if not inGame:
		await GameData.joinGame(self.gameID, $MarginContainer/VBoxContainer/GameEditor/MarginContainer/VBoxContainer/Passworded/PasswordText.text)
	else:
		if await GameData.leaveGame(self.gameID):
			GameData.gotoCurrent()
	$MarginContainer/VBoxContainer/GameEditor.getActivationButton().disabled = false
