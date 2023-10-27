extends MarginContainer

var game

var gameID

var chats = {}

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID

	refresh(await GameData.loadChats(gameID))
	
func refresh(newChats):
	newChats.sort_custom(func(a, b): return a.readTimestamp > b.readTimestamp)
	
	var pos = 0
	for chat in newChats:
		if chats.has(chat.id):
			$VBoxContainer/ScrollContainer/ChatContainer.move_child(chats[chat.id], pos)
			pos += 1
			continue
		
		var userChat = preload("res://UserChat.tscn").instantiate()
		
		userChat.init(gameID, chat.id)
		userChat.selected.connect(selected)
		userChat.deselected.connect(deselected)
		
		chats[chat.id] = userChat
		
		$VBoxContainer/ScrollContainer/ChatContainer.add_child(userChat)
		$VBoxContainer/ScrollContainer/ChatContainer.move_child(userChat, pos)
		pos += 1
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func chatChanged(chatID: int):
	if GameData.chatInGame(chatID, gameID):
		refresh(GameData.getChats(gameID))

func selected(conversation):
	$ChatPanel.add_child(conversation)

func deselected(conversation):
	$ChatPanel.remove_child(conversation)

func _on_new_chat_pressed():
	var conversation = preload("res://Conversation.tscn").instantiate()
	conversation.initTemp(gameID)
	
	selected(conversation)
	conversation.deselected.connect(deselected)
