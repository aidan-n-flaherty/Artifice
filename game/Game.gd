extends Control

var gameID: int

var game: GameInterface

var menuDisplay

var detailDisplay

var viewingEnd = false

# Called when the node enters the scene tree for the first time.
func _ready():
	$Viewport/Viewport3D/CameraPivot/FloorSprite.material_override.set_shader_parameter("screen_texture", $Viewport/Viewport3D/CameraPivot/FloorDisplay.get_texture())
	#$Viewport/Viewport3D/CameraPivot/Terrain.get_surface_override_material(0).set_shader_parameter("screen_texture", $Viewport/Viewport3D/CameraPivot/SubViewport.get_texture())
	
func init(gameID):
	self.gameID = gameID
	
	game = GameData.getGame(gameID)
	game.set_process(true)
	
	game.connect("addOrder", addOrder)
	game.connect("selectVessel", selectVessel)
	game.connect("selectOutpost", selectOutpost)
	game.connect("selectSpecialist", selectSpecialist)
	game.connect("deselect", deselect)
	game.connect("deselectSpecialist", deselectSpecialist)

	var startTime = int(Time.get_unix_time_from_system())
	
	$Viewport/Viewport3D.add_child(game)
	
	$Viewport/GameOverlay/Overlay/VBoxContainer/Timeline.init(gameID)
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Status.init(gameID)
	$Viewport/Viewport3D/CameraPivot.init(gameID)
	
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Shop.init(gameID)
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Chat.init(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not game.hasStarted():
		$Viewport/GameOverlay/Overlay/NotStarted.show()
		if (game.getStartTime() - game.getTime()) < (2 * 365 * 24 * 60 * 60): #if less than 2 years
			$Viewport/GameOverlay/Overlay/NotStarted/Label.text = "Game starts in " + Utilities.timeToStr(game.getStartTime() - game.getTime())
		else:
			var details = GameData.getGameDetails(gameID)
			$Viewport/GameOverlay/Overlay/NotStarted/Label.text = "Waiting for players (" + str(details.gameData.playerCount) + "/" + str(details.gameSettings.playerCap) + ")"
	else:
		$Viewport/GameOverlay/Overlay/NotStarted.hide()

	if game.getSelectedUnits() >= 0:
		$Viewport/GameOverlay/Overlay/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/Label.text = str(floor(game.getSelectedUnits() * $Viewport/GameOverlay/Overlay/VBoxContainer/HBoxContainer/Control/PercentBar.value))
	else:
		$Viewport/GameOverlay/Overlay/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/Label.text = ""
		
	if game.hasEnded():
		if not viewingEnd:
			viewingEnd = true
	
			if not GameData.isFinished(gameID):
				if await GameData.verifyEnd(gameID):
					await GameData.viewEnd(gameID)
					$Viewport/GameOverlay/Overlay/EndGame.init(gameID)
					$Viewport/GameOverlay/Overlay/EndGame.show()
	else:
		viewingEnd = false
		$Viewport/GameOverlay/Overlay/EndGame.hide()
		
func addOrder(type, referenceID, timestamp, arguments):
	GameData.addOrder(gameID, type, referenceID, timestamp, arguments)

func setDisplay(scene):
	if detailDisplay != null:
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/VBoxContainer/MarginContainer.remove_child(detailDisplay)
	
	detailDisplay = scene

	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/VBoxContainer/MarginContainer.add_child(detailDisplay)
	$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.show()

func selectVessel(vessel):
	var scene = preload("res://VesselDetails.tscn").instantiate()
	scene.init(vessel, gameID)
	scene.battleForecastToggle.connect(vesselBattleForecast)
	$Viewport/Viewport3D/CameraPivot.selected(vessel)
	setDisplay(scene)

func vesselBattleForecast(vessel):
	if $"Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Battle Forecast".visible:
		setMenuDisplay(null, false)
	else:
		var scene = $"Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/Battle Forecast"
		scene.queue_free()
		$"Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/".remove_child(scene)
		
		scene = preload("res://Battle Forecast.tscn").instantiate()
		scene.init(gameID, vessel.getID())
		$"Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/".add_child(scene)
		setMenuDisplay(scene, true)
	

func selectOutpost(outpost):
	var scene = preload("res://OutpostDetails.tscn").instantiate()
	scene.init(outpost, gameID)
	$Viewport/Viewport3D/CameraPivot.selected(outpost)
	setDisplay(scene)
	
func selectSpecialist(specialist):
	var scene = preload("res://SpecialistDetails.tscn").instantiate()
	scene.init(specialist, gameID)
	setDisplay(scene)
	
func deselect():
	if(detailDisplay):
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/VBoxContainer/MarginContainer.remove_child(detailDisplay)
		detailDisplay= null
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.hide()

func deselectSpecialist(specialist):
	if detailDisplay and "specialistID" in detailDisplay and detailDisplay.specialistID == specialist:
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel/VBoxContainer/MarginContainer.remove_child(detailDisplay)
		detailDisplay= null
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/ElementDisplay/VBoxContainer/Panel.hide()

func _on_percent_bar_value_changed(value):
	game.setPercent(value);

func setMenuDisplay(scene, select):
	if menuDisplay != null:
		#$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/AnimationPlayer.play("menu_close")
		#await get_tree().create_timer(0.4).timeout
		menuDisplay.hide()
		menuDisplay = null
	
	if select:
		menuDisplay = scene
		#await get_tree().create_timer(0.4).timeout
		menuDisplay.show()
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel.show()
		$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/AnimationPlayer.play("slide_up")
	else:
		#$Viewport/GameOverlay/Overlay/UIOverlay/Separator/TabDisplay/Panel/AnimationPlayer.play("menu_close")
		#await get_tree().create_timer(0.4).timeout
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


func _on_back_button_pressed():
	$Viewport/Viewport3D.remove_child(game)
	$Viewport/Viewport3D/CameraPivot/FloorDisplay.remove_child(game.getFloorDisplay())
	game.set_process(false)
	
	GameData.goto_scene("res://MainMenu.tscn")


func _on_camera_pivot_unselect():
	setMenuDisplay(null, false)
	
	$MenuBar/VSplitContainer/Tabs/HBoxContainer/StatusContainer/StatusButton.button_pressed = false
	$MenuBar/VSplitContainer/Tabs/HBoxContainer/ChatContainer/ChatButton.button_pressed = false
	$MenuBar/VSplitContainer/Tabs/HBoxContainer/ShopContainer/ShopButton.button_pressed = false
	$MenuBar/VSplitContainer/Tabs/HBoxContainer/LogContainer/LogsButton.button_pressed = false
