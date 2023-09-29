extends Control

var gameID: int

var game: GameInterface

var startTime

var menuDisplay

var detailDisplay

# Called when the node enters the scene tree for the first time.
func _ready():
	game = GameData.getGame(gameID)
	game.connect("addOrder", addOrder)
	game.connect("selectVessel", selectVessel)
	game.connect("selectOutpost", selectOutpost)
	game.connect("selectSpecialist", selectSpecialist)
	game.connect("deselect", deselect)
	game.connect("deselectSpecialist", deselectSpecialist)

	startTime = int(Time.get_unix_time_from_system())
	
	print(game)
	$Viewport/Viewport3D.add_child(game)
	$Viewport/GameOverlay/Overlay/VBoxContainer/Timeline.init(gameID)
	$Viewport/Viewport3D/CameraPivot.game = game
	$Viewport/Viewport3D/CameraPivot/FloorDisplay.add_child(game.getFloorDisplay())
	
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Shop.init(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func addOrder(type, referenceID, timestamp, arguments):
	print(game.getTime())
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": type,
		"referenceID": referenceID,
		"timestamp": timestamp,
		"argumentIDs": arguments
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")

func setDisplay(scene):
	if detailDisplay != null:
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/MarginContainer.remove_child(detailDisplay)
	detailDisplay = scene

	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/MarginContainer.add_child(detailDisplay)
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.show()

func selectVessel(vessel):
	var scene = preload("res://VesselDetails.tscn").instantiate()
	scene.init(vessel, gameID)
	setDisplay(scene)

func selectOutpost(outpost):
	var scene = preload("res://OutpostDetails.tscn").instantiate()
	scene.init(outpost, gameID)
	setDisplay(scene)
	
func selectSpecialist(specialist):
	var scene = preload("res://SpecialistDetails.tscn").instantiate()
	scene.init(specialist, gameID)
	setDisplay(scene)
	
func deselect():
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/MarginContainer.remove_child(detailDisplay)
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.hide()

func deselectSpecialist(specialist):
	if detailDisplay and "specialistID" in detailDisplay and detailDisplay.specialistID == specialist:
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/MarginContainer.remove_child(detailDisplay)
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.hide()

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
