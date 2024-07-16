extends MarginContainer
class_name GameList

var gameIDs

var buttons = {}

var suppress = false
	
# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.gamesChanged.connect(gamesChanged)
	
	if not suppress:
		init()
	
func suppressReady():
	suppress = true
	
func initList():
	pass
	
func init():
	await initList()
	
	gameIDs.sort_custom(func(a, b):
		var gameA = GameData.getGameDetails(a)
		var gameB = GameData.getGameDetails(b)
		return gameA != null and gameB != null && gameA.gameData.createdAt > gameB.gameData.createdAt
	)
	
	var node = $MarginContainer/VBoxContainer/ScrollContainer/Content
	
	var ids = buttons.keys()
	for id in ids:
		if not gameIDs.has(id):
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
