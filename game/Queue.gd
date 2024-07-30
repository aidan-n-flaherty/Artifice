extends MarginContainer

var queueName = ""

func init(queueName: String):
	self.queueName = queueName
	
	setValues()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func setValues():
	var queue = GameData.getQueue(queueName)

	var inQueue = queue.joinTime >= 0
	
	$Selected.visible = inQueue
	$Unselected.visible = not inQueue
	$Joining.visible = false
	
	$Margin/VBoxContainer/Title.text = queue.queueName
	$Margin/VBoxContainer/Players.text = str(queue.playerCount) + " out of " + str(queue.playerCap)

func _on_selected_pressed():
	GameData.leaveQueue(queueName)

func _on_unselected_pressed():
	$Margin/VBoxContainer/Title.text = "Joining Queue..."
	
	$Unselected.visible = false
	$Joining.visible = true
	
	GameData.joinQueue(queueName)
