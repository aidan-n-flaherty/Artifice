extends MarginContainer

var game

var gameID

var chats

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	chats = await GameData.loadChats(gameID)
	
	for chat in chats:
		var userChat = preload("res://UserChat.tscn").instantiate()
		
		userChat.init(chat)
		
		$VBoxContainer/ScrollContainer/ChatContainer.add_child(userChat)
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
