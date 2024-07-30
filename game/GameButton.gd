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
	
	$Margin/VBoxContainer/HBoxContainer/Title.text = gameDetails.gameSettings.lobbyName
	$Margin/VBoxContainer/HBoxContainer/Players.text = str(gameDetails.gameData.playerCount) + "/" + str(gameDetails.gameSettings.playerCap)
	
	if gameDetails.gameData.started:
		$Margin/VBoxContainer/HBoxContainer2/Start.text = "Started on " + Utilities.timeToDateStr(gameDetails.gameData.startTime)
	else:
		$Margin/VBoxContainer/HBoxContainer2/Start.text = "Created on " + Utilities.timeToDateStr(gameDetails.gameData.createdAt)
	
	for node in $Margin/VBoxContainer/HBoxContainer2/Icons.get_children():
		node.hide()
	
	if gameDetails.gameData.hasPassword and not alreadyJoined:
		$Margin/VBoxContainer/HBoxContainer2/Icons/Password.show()
	
	if alreadyJoined and gameDetails.gameData.hasNotifications:
		$Panel.show()
		$Margin/VBoxContainer/HBoxContainer2/Icons/Notifications.show()
	
	if alreadyJoined and gameDetails.gameData.hasChatNotifications:
		$Margin/VBoxContainer/HBoxContainer2/Icons/ChatNotifications.show()
	
	if gameDetails.gameSettings.ranked:
		$Margin/VBoxContainer/HBoxContainer2/Icons/Ranked.show()

func _on_button_pressed():
	if alreadyJoined and int(gameDetails.gameData.startTime) < Time.get_unix_time_from_system() + (2 * 365 * 24 * 60 * 60):
		GameData.viewGame(gameID, past)
	else:
		GameData.viewGameDetail(gameID)
