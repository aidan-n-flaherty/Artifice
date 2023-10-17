extends MarginContainer

signal deselected(conversation)

var chat

var participants

var messageIDs = []

var messages = []

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)

func init(chat, participants):
	self.chat = chat
	self.participants = participants
	
	refresh(chat.messages)
	
func refresh(messages):
	chat.messages.sort_custom(func(a, b): return a.timestamp < b.timestamp)
	for message in chat.messages:
		if messageIDs.has(message.id):
			continue
		
		messageIDs.push_back(message.id)
		
		var messageNode = preload("res://Message.tscn")
		messageNode.init(message)
		
		messages.push_back(message)
		
		if len(messages) > 0:
			messages.back().add_sibling(messageNode)
		else:
			$MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.add_child(messageNode)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func chatChanged(chatID: int):
	if chatID == chat.id:
		refresh(chat.messages)

func _on_back_pressed():
	emit_signal("deselected", self)
