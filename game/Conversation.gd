extends MarginContainer

signal deselected(conversation)

var chatID

var chat

var participants

var messages = {}

var gameID

var game

var temporary

var playerTags = []

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)

func initTemp(gameID):
	temporary = true
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for player in game.getPlayers():
		var playerTag = preload("res://PlayerTag.tscn").instantiate()
		
		playerTag.init(player, false, true)
		playerTags.push_back(playerTag)
		$MarginContainer/VBoxContainer/PlayerList.add_child(playerTag)

func init(gameID, chatID):
	temporary = false
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for tag in playerTags:
		$MarginContainer/VBoxContainer/PlayerList.remove_child(tag)
		tag.queue_free()
	playerTags.clear()
	
	chat = GameData.getChat(chatID)
	self.chatID = chatID
	
	for player in game.getPlayers():
		if PackedInt32Array(chat.participants).has(player.getUserID()):
			var playerTag = preload("res://PlayerTag.tscn").instantiate()
		
			playerTag.init(player, true, false)
			playerTags.push_back(playerTag)
			$MarginContainer/VBoxContainer/PlayerList.add_child(playerTag)
	
	refresh(chat.messages)
	
func refresh(messageList):
	messageList.sort_custom(func(a, b): return a.timestamp < b.timestamp)
	
	for index in len(messageList):
		var message = messageList[index]
		
		var messageNode
		
		if messages.has(message.id):
			messageNode = messages[message.id]
		else:
			messageNode = preload("res://Message.tscn").instantiate()
			messageNode.init(message)
			
			messages[message.id] = messageNode
			
			$MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.add_child(messageNode)
		
		$MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.move_child(messageNode, index)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var canSend = false
	for tag in playerTags:
		if tag.isSelected():
			canSend = true
			break
	
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = not canSend

func chatChanged(chatID: int):
	if chat and chatID == chat.id:
		refresh(chat.messages)
	else:
		init(gameID, chatID)

func _on_back_pressed():
	emit_signal("deselected", self)

func _on_send_pressed():
	if $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text == "":
		return
	
	if !chatID:
		var playerIDs = PackedInt32Array()
		for tag in playerTags:
			if tag.isSelected():
				playerIDs.push_back(tag.getUserID())
		
		var tmp = await GameData.createChat(gameID, playerIDs)

		if tmp:
			chatID = int(tmp)
		else:
			return
	
	if await GameData.sendMessage(chatID, $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text):
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.clear()
