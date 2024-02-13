extends GameList

var queues

# Called when the node enters the scene tree for the first time.
func _ready():
	super()
	
func initList():
	GameData.loadOngoingGames()
	gameIDs = GameData.getOpenGames()
	
	GameData.queuesChanged.connect(updateQueues)
	updateQueues()
	
func generateButton(id: int):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://JoinButton.tscn").instantiate()
	button.init(id)
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func updateQueues():
	queues = await GameData.loadQueues()
	
	$VBoxContainer/RankedQueue/RankedSelected.visible = queues.has("ranked")
	$VBoxContainer/RankedQueue/RankedUnselected.visible = not queues.has("ranked")
	
	if(queues.has("ranked")):
		$VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked), " + str(queues["ranked"].playerCount) + " out of n"
	else:
		$VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked)"


func _on_ranked_selected_pressed():
	GameData.leaveQueue("ranked")


func _on_ranked_unselected_pressed():
	GameData.joinQueue("ranked")
