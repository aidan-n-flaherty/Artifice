extends GameList

# Called when the node enters the scene tree for the first time.
func _ready():
	super()
	
func initList():
	GameData.loadOngoingGames()
	gameIDs = GameData.getOpenGames()
	
func generateButton(id: int):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://JoinButton.tscn").instantiate()
	button.init(id)
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
