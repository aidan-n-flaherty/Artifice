extends GameList

var queues = {}

func _ready():
	super._ready()
	
	GameData.queuesChanged.connect(updateQueues)
	
func initList():
	gameIDs = GameData.getOpenGames()
	
	updateQueues()
	
	GameData.loadQueues()

func init():
	await super.init()
	await GameData.loadOpenGames()
	await super.init()
	
func generateButton(id: int):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id, false)
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func updateQueues():
	var queueNames = GameData.getQueues().keys()
	
	for queueName in queueNames:
		var node
		
		if not queues.has(queueName):
			node = preload("res://Queue.tscn").instantiate()
			self.queues[queueName] = node
			$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/GridContainer.add_child(node)
		else:
			node = self.queues[queueName]
		
		node.init(queueName)
