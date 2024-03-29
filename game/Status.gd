extends Control

var gameID

var game

var playerStatuses = {}


func update():

	var players = game.getSortedPlayers()
	var win = 200
	var largestCapacity = 0
	
	for p in players: #determine largest capacity
		var capacity = p.getCapacity()
		if(capacity > largestCapacity):
			largestCapacity = capacity
	
	var i = 0
	for p in players:
		
		var username = p.getName()
		var units = p.getUnits()
		var capacity = p.getCapacity()
		
		var outposts = p.getOutposts()
		var resources = p.getResources()
		
		var factories = p.getFactories()
		var mines = p.getMines()
		var generators = p.getGenerators()
		var color = p.getColor()
		
		var playerStatus
		
		if playerStatuses.has(p.getUserID()):
			playerStatus = playerStatuses[p.getUserID()]
			playerStatus.init(game, p, username, units, capacity, resources, largestCapacity, win, outposts, factories, generators, mines, color)
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.move_child(playerStatus,i)
	
		else:
			playerStatus = preload("res://PlayerStatus.tscn").instantiate()
			playerStatus.init(game, p, username, units, capacity, resources, largestCapacity, win, outposts, factories, generators, mines, color)
			playerStatuses[p.getUserID()] = playerStatus
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.add_child(playerStatus)
		i += 1
		
		
		
	
	
# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	update()
	#default to sort by wincon
	#call the sort function/get the array of names
	#iterate through them on the menu
	#display their stats next to them


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	update()


func _on_surrender_button_pressed():
	GameData.addOrder(gameID, "SURRENDER", int(game.getReferenceID()), game.getTime(), [])
