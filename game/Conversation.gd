extends MarginContainer

signal deselected(conversation)

var chatID

var chat

var participants

var messages = {}

var gameID

var game

var temporary

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)

func initTemp(gameID):
	temporary = true
	
	game = GameData.getGame(gameID)
	self.gameID = gameID

func init(gameID, chatID, participants):
	temporary = false
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	chat = GameData.getChat(chatID)
	self.chatID = chatID
	self.participants = participants
	
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
	pass

func chatChanged(chatID: int):
	if chat and chatID == chat.id:
		refresh(chat.messages)

func _on_back_pressed():
	emit_signal("deselected", self)

func _on_button_pressed():
	if $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text == "":
		return
	
	if !chatID:
		chatID = await GameData.makeChat()
	
	if await GameData.sendMessage(chat.id, $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text):
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.clear()
