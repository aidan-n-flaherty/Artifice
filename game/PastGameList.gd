extends GameList

# Called when the node enters the scene tree for the first time.
func _ready():
	super()
	
func initList():
	gameIDs = GameData.getPastGames()
	
func generateButton(id):
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id)
	return button


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
