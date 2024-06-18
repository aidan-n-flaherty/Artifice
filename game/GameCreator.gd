extends GameEditor

# Called when the node enters the scene tree for the first time.
func _ready():
	setEditable(true)
	playersVisible(false)
	
	super._ready()

func activate():
	var game = await createGame()

	if game:
		GameData.currentTab = "res://CurrentGameList.tscn"
		GameData.goto_scene("res://MainMenu.tscn")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
