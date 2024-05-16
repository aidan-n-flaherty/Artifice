extends MarginContainer

var gameID

var gameDetails

var alreadyJoined

var past

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID: int, alreadyJoined: bool, past=false):
	GameData.gameChanged.connect(update)
	
	self.alreadyJoined = alreadyJoined
	self.past = past
	
	self.gameID = gameID
	update(self.gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func update(gameID):
	if gameID != self.gameID:
		return
	
	gameDetails = GameData.getGameDetails(gameID)
	
	$Margin/HBoxContainer/Title.text = gameDetails.gameSettings.lobbyName
	$Margin/HBoxContainer/Players.text = str(gameDetails.gameData.playerCount) + "/" + str(gameDetails.gameSettings.playerCap)
	
	for node in $Margin/HBoxContainer/Icons.get_children():
		node.hide()
	
	if gameDetails.gameData.hasPassword and not alreadyJoined:
		$Margin/HBoxContainer/Icons/Spacer.show()
		$Margin/HBoxContainer/Icons/Password.show()
	
	if alreadyJoined and gameDetails.gameData.hasNotifications:
		$Panel.show()
		$Margin/HBoxContainer/Icons/Spacer.show()
		$Margin/HBoxContainer/Icons/Notifications.show()
	
	if alreadyJoined and gameDetails.gameData.hasChatNotifications:
		$Panel.show()
		$Margin/HBoxContainer/Icons/Spacer.show()
		$Margin/HBoxContainer/Icons/ChatNotifications.show()
	
	if gameDetails.gameSettings.ranked:
		$Margin/HBoxContainer/Icons/Spacer.show()
		$Margin/HBoxContainer/Icons/Ranked.show()

func _on_button_pressed():
	if alreadyJoined and int(gameDetails.gameData.startTime) < Time.get_unix_time_from_system() + (2 * 365 * 24 * 60 * 60):
		GameData.viewGame(gameID, past)
	else:
		GameData.viewGameDetail(gameID)
