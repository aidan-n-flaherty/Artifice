extends GameList

# Called when the node enters the scene tree for the first time.
func _ready():
	gameIDs = GameData.getOngoingGames()

	super()

func generateButton(id) -> Button:
	var button = Button.new()
	button.text = str("Game: ", str(id))
	button.connect("pressed", GameData.viewGame.bind(id))
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
