extends GameEditor

signal gameModified

var elapsed = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	super._ready()

func init(gameID):
	setEditable(true)
	deserialize(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func activate():
	if await HTTPManager.putReq("/editMatch", serialize(), { "gameID": self.gameID }):
		GameData.loadGameSettings(self.gameID)
