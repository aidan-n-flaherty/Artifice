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

func init(gameID, chatID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	self.chatID = chatID
	chat = GameData.getChat(chatID)
	
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

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_button_pressed():
	var conversation = preload("res://Conversation.tscn").instantiate()
	conversation.init(gameID, chatID, participants)
	conversation.deselected.connect(deselectedConversation)
	
	emit_signal("selected", conversation)
	
func deselectedConversation(conversation):
	emit_signal("deselected", conversation)
