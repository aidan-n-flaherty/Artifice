extends Control

var gameID
var game

var startTime

var menuDisplay

# Called when the node enters the scene tree for the first time.
func _ready():
	game = GameData.getGame(gameID)
	game.connect("addOrder", addOrder)

	startTime = int(Time.get_unix_time_from_system())
	
	$Viewport/Viewport3D.add_child(game)
	$Viewport/GameOverlay/Overlay/VBoxContainer/Timeline.game = game
	$Viewport/Viewport3D/CameraPivot.game = game
	$Viewport/Viewport3D/CameraPivot/FloorDisplay.add_child(game.getFloorDisplay())
	
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Shop.init(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
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

func setMenuDisplay(scene, select):
	if menuDisplay != null:
		menuDisplay.hide()
		menuDisplay = null
	
	if select:
		menuDisplay = scene
		menuDisplay.show()
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel.show()
	else:
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel.hide()

func _on_status_button_toggled(button_pressed):
	setMenuDisplay($Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Status,
		button_pressed)


func _on_chat_button_toggled(button_pressed):
	setMenuDisplay($Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Chat,
		button_pressed)


func _on_shop_button_toggled(button_pressed):
	setMenuDisplay($Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Shop,
		button_pressed)


func _on_logs_button_toggled(button_pressed):
	setMenuDisplay($Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Logs,
		button_pressed)
