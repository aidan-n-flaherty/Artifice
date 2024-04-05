extends GameEditor

# Called when the node enters the scene tree for the first time.
func _ready():
	setEditable(true)
	
	super._ready()

func activate():
	var game = await super.activate()
	
	if game:
		GameData.viewGame(game.gameData.id)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
