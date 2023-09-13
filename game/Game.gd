extends Control

var gameID
var game

var viewing

var startTime

# Called when the node enters the scene tree for the first time.
func _ready():
	game = GameData.getGame(gameID)
	game.connect("addOrder", addOrder)

	startTime = int(Time.get_unix_time_from_system())
	
	$Viewport/Viewport3D.add_child(game)
	$Viewport/VSplitContainer/VBoxContainer/Timeline.game = game
	$Viewport/Viewport3D/CameraPivot.game = game
	$Viewport/Viewport3D/CameraPivot/FloorDisplay.add_child(game.getFloorDisplay())

	viewing = $Viewport
	viewing.show()
	for child in viewing.get_children():
		child.show()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setView(node):
	viewing.hide()
	for child in viewing.get_children():
		child.hide()
		
	viewing = node
	
	viewing.show()
	for child in viewing.get_children():
		child.show()

func _on_game_pressed():
	setView($Viewport)

func _on_chat_pressed():
	setView($Chat)
	
func _on_hire_pressed():
	setView($Hire)

func _on_status_pressed():
	setView($Status)

func _on_logs_pressed():
	setView($Logs)
	
func addOrder(type, referenceID, timestamp, arguments):
	print(timestamp + 2 - int(Time.get_unix_time_from_system()))
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": type,
		"referenceID": referenceID,
		"timestamp": timestamp + 2,
		"argumentIDs": arguments
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), int(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")


func _on_percent_bar_value_changed(value):
	game.setPercent(value);
