extends MarginContainer

signal selected(conversation)

signal deselected(conversation)

var chatID

var chat

var participants

var gameID

var game

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID: int, chatID: int):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	self.chatID = chatID
	chat = GameData.getChat(chatID)
	
	chat.messages.sort_custom(func(a, b): return a.timestamp < b.timestamp)
	var lastMessage = chat.messages[len(chat.messages) - 1] if len(chat.messages) > 0 else null
	
	if lastMessage:
		var user = await GameData.getUser(lastMessage.senderID)
		
		if user:
			$MarginContainer/VBoxContainer/LastMessage.text = user.username + ": " + lastMessage.content
		
		if lastMessage.senderID != GameData.getSelfID() and lastMessage.timestamp > chat.readTimestamp:
			$Button.modulate = Color.DARK_GRAY
		else:
			$Button.modulate = Color.BLACK
	
	participants = []
	
	$MarginContainer/VBoxContainer/Participants.text = ""
	for i in len(chat.participants):
		var user = await GameData.getUser(chat.participants[i])
		participants.push_back(user)
		
		if i > 0:
			$MarginContainer/VBoxContainer/Participants.text += ", "
		if len(chat.participants) >= 2 and i == len(chat.participants) - 1:
			$MarginContainer/VBoxContainer/Participants.text += "and "
		$MarginContainer/VBoxContainer/Participants.text += user.username
	
	if chat.public:
		$MarginContainer/VBoxContainer/Participants.text = "Public Chat: " + $MarginContainer/VBoxContainer/Participants.text

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_button_pressed():
	var conversation = preload("res://Conversation.tscn").instantiate()
	conversation.init(gameID, chatID)
	conversation.deselected.connect(deselectedConversation)
	
	emit_signal("selected", conversation)
	
func deselectedConversation(conversation):
	init(gameID, chatID)
	emit_signal("deselected", conversation)
