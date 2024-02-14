extends GameList

var queues

# Called when the node enters the scene tree for the first time.
func _ready():
	super()
	
func initList():
	GameData.loadOpenGames()
	gameIDs = GameData.getOpenGames()
	
	GameData.queuesChanged.connect(updateQueues)
	
	setValues()
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
	await GameData.loadQueues()
	
	setValues()
	
func setValues():
	var quickRanked = GameData.getQueue("quickRanked")
	
	$VBoxContainer/RankedQueue/RankedSelected.visible = quickRanked != null
	$VBoxContainer/RankedQueue/RankedUnselected.visible = quickRanked == null
	$VBoxContainer/RankedQueue/RankedJoining.visible = false
	
	if quickRanked:
		$VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked), " + str(quickRanked.playerCount) + " out of " + str(quickRanked.playerCap)
	else:
		$VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked)"


func _on_ranked_selected_pressed():
	GameData.leaveQueue("quickRanked")


func _on_ranked_unselected_pressed():
	$VBoxContainer/RankedQueue/Margin/Title.text = "Joining Queue..."
	
	$VBoxContainer/RankedQueue/RankedUnselected.visible = false
	$VBoxContainer/RankedQueue/RankedJoining.visible = true
	
	GameData.joinQueue("quickRanked")
