extends MarginContainer

var gameID

var game

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(gameID):
	self.gameID = gameID
	game = GameData.getGame(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
