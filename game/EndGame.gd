extends Panel

var game: GameInterface

var gameID: int

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID: int):
	self.game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.get_children():
		child.queue_free()
	
	for player in game.getSortedPlayers():
		var playerDetail = preload("res://PlayerDetail.tscn").instantiate()
		playerDetail.init(player.getName(), game.getScore(player.getUserID()))
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.add_child(playerDetail)
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
