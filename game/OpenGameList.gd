extends GameList

var queues
	
func initList():
	GameData.loadOpenGames()
	gameIDs = GameData.getOpenGames()
	
	GameData.queuesChanged.connect(updateQueues)
	
	setValues()
	updateQueues()
	
func generateButton(id: int):
	var gameDetails = GameData.getGameDetails(id)
	var button = preload("res://GameButton.tscn").instantiate()
	button.init(id, false)
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func updateQueues():
	await GameData.loadQueues()
	
	setValues()
	
func setValues():
	var quickRanked = GameData.getQueue("quickRanked")
	
	$MarginContainer/VBoxContainer/RankedQueue/RankedSelected.visible = quickRanked != null
	$MarginContainer/VBoxContainer/RankedQueue/RankedUnselected.visible = quickRanked == null
	$MarginContainer/VBoxContainer/RankedQueue/RankedJoining.visible = false
	
	if quickRanked:
		$MarginContainer/VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked), " + str(quickRanked.playerCount) + " out of " + str(quickRanked.playerCap)
	else:
		$MarginContainer/VBoxContainer/RankedQueue/Margin/Title.text = "Quick Play (Ranked)"


func _on_ranked_selected_pressed():
	GameData.leaveQueue("quickRanked")


func _on_ranked_unselected_pressed():
	$MarginContainer/VBoxContainer/RankedQueue/Margin/Title.text = "Joining Queue..."
	
	$MarginContainer/VBoxContainer/RankedQueue/RankedUnselected.visible = false
	$MarginContainer/VBoxContainer/RankedQueue/RankedJoining.visible = true
	
	GameData.joinQueue("quickRanked")
