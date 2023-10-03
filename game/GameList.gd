extends MarginContainer
class_name GameList

var gameIDs

var buttons = {}
	
# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.gamesChanged.connect(gamesChanged)
	
	init()
	
func initList():
	pass
	
func init():
	initList()
	
	var node = $ScrollContainer/Content
	
	var ids = buttons.keys()
	for id in ids:
		if not gameIDs.contains(id):
			buttons[id].queue_free()
			buttons.erase(id)
	
	for id in gameIDs:
		if ids.has(id): continue
		
		var button = generateButton(id)
		buttons[id] = button
		node.add_child(button)

func generateButton(id: int):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id)
	
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func gamesChanged():
	init()
