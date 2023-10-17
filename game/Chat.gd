extends MarginContainer

var game

var gameID

var chats = []

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID

	refresh(await GameData.loadChats(gameID))
	
func refresh(chats):
	chats.sort_custom(func(a, b): return a.readTimestamp > b.readTimestamp)
	
	for chat in chats:
		if self.chats.has(chat):
			continue
		
		var userChat = preload("res://UserChat.tscn").instantiate()
		
		userChat.init(chat)
		userChat.selected.connect(selected)
		userChat.deselected.connect(deselected)
		
		$VBoxContainer/ScrollContainer/ChatContainer.add_child(userChat)
	
	self.chats = chats
	
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
