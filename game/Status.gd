extends Control

var gameID

var game

var specialists

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	#default to sort by wincon
	var players
	
	
	
	#call the sort function/get the array of names
	#iterate through them on the menu
	#display their stats next to them


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	
	pass
