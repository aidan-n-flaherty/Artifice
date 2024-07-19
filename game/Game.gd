extends Control

var gameID: int

var game: GameInterface

var menuDisplay

var detailDisplay

var viewingEnd = false

var hasLost = false

var menuButtons = []

var userID: int

var diff = 0

var offline: bool

# Called when the node enters the scene tree for the first time.
func _ready():	
	get_viewport().connect("size_changed", resize)
	
	resize()

func resize():
	var element = elementDisplay()
	var tab = tabDisplay()
	if get_viewport().size.x > get_viewport().size.y:
		element.get_parent().remove_child(element)
		tab.get_parent().remove_child(tab)
		
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator.add_child(element)
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator.add_child(tab)
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator.move_child($Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator/Spacer, 2)
	else:
		element.get_parent().remove_child(element)
		tab.get_parent().remove_child(tab)
		
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/VSeparator.add_child(element)
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/VSeparator.add_child(tab)

	if get_viewport().size.x > get_viewport().size.y:
		if $Viewport/GameOverlay/MarginContainer/VTimeline.get_child_count() > 0:
			var timeline = $Viewport/GameOverlay/MarginContainer/VTimeline.get_child(0)
			timeline.setVertical()
			
			$Viewport/GameOverlay/MarginContainer/VTimeline.remove_child(timeline)
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/HTimeline.add_child(timeline)
	else:
		if $Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/HTimeline.get_child_count() > 0:
			var timeline = $Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/HTimeline.get_child(0)
			timeline.setHorizontal()
			
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/HTimeline.remove_child(timeline)
			$Viewport/GameOverlay/MarginContainer/VTimeline.add_child(timeline)
	
	if get_viewport().size.x > get_viewport().size.y:
		if $Viewport/GameOverlay/VMenuBar.get_child_count() > 0:
			var tabs = $Viewport/GameOverlay/VMenuBar.get_child(0)
			
			tabs.get_node("Horizontal").show()
			tabs.get_node("Vertical").hide()
			
			tabs.add_theme_constant_override("margin_top", 40)
			tabs.add_theme_constant_override("margin_left", 0)
			
			for child in tabs.get_node("HBoxContainer").get_children():
				tabs.get_node("HBoxContainer").remove_child(child)
				tabs.get_node("VBoxContainer").add_child(child)
			
			$Viewport/GameOverlay/VMenuBar.remove_child(tabs)
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.add_child(tabs)
	else:
		if $Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.get_child_count() > 0:
			var tabs = $Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.get_child(0)
			
			tabs.get_node("Horizontal").hide()
			tabs.get_node("Vertical").show()
			
			tabs.add_theme_constant_override("margin_top", 0)
			tabs.add_theme_constant_override("margin_left", 40)
			
			for child in tabs.get_node("VBoxContainer").get_children():
				tabs.get_node("VBoxContainer").remove_child(child)
				tabs.get_node("HBoxContainer").add_child(child)
				
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.remove_child(tabs)
			$Viewport/GameOverlay/VMenuBar.add_child(tabs)
	
	if OS.get_name() == "iOS" or OS.get_name() == "Android":
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/DisplaySpacerL.custom_minimum_size.x = DisplayServer.get_display_safe_area().position.x
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/DisplaySpacerR.custom_minimum_size.x = DisplayServer.screen_get_size().x - (DisplayServer.get_display_safe_area().position.x + DisplayServer.get_display_safe_area().size.x)

func elementDisplay():
	return get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator/ElementDisplay") if get_node_or_null("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator/ElementDisplay") else get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/VSeparator/ElementDisplay")

func tabDisplay():
	return get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator/TabDisplay") if get_node_or_null("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/HSeparator/TabDisplay") else get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay/VSeparator/TabDisplay")

func init(gameID: int, offline=false):
	self.gameID = gameID
	self.offline = offline
	
	game = GameData.getGame(gameID)
	if offline:
		game.setOffline()
	game.resume()
	game.set_process(true)
	game.set_visible(true)
	
	game.connect("addOrder", addOrder)
	game.connect("replaceOrder", replaceOrder)
	game.connect("selectVessel", selectVessel)
	game.connect("selectOutpost", selectOutpost)
	game.connect("selectSpecialist", selectSpecialist)
	game.connect("deselect", deselect)
	game.connect("deselectSpecialist", deselectSpecialist)

	var startTime = int(Time.get_unix_time_from_system())
	
	$Viewport/Viewport3D.add_child(game)
	
	$Viewport/GameOverlay/VMenuBar/Tabs/HBoxContainer/ShopContainer.visible = game.getUserGameID() != -1
	$Viewport/GameOverlay/VMenuBar/Tabs/HBoxContainer/ChatContainer.visible = game.getUserGameID() != -1 and not game.isOffline()
	$Viewport/GameOverlay/VMenuBar/Tabs/HBoxContainer/EditorContainer.visible = not game.isOffline()
	
	$Viewport/GameOverlay/MarginContainer/VTimeline/Timeline.init(gameID)
	$Viewport/Viewport3D/CameraManager.init(gameID)
	
	tabDisplay().get_node("Panel/Status").init(gameID)
	tabDisplay().get_node("Panel/Shop").init(gameID)
	tabDisplay().get_node("Panel/Orders").init(gameID)
	
	var details = GameData.getGameDetails(gameID)
	
	
	if not game.isOffline():
		tabDisplay().get_node("Panel/Chat").init(gameID)
	
		if tabDisplay().get_node_or_null("Panel/GameEditor"):
			tabDisplay().remove_child(tabDisplay().get_node("Panel/GameEditor"))
		
		var gameEditor = ResourceLoader.load("res://GameModifier.tscn").instantiate()
		gameEditor.name = "GameEditor"
		gameEditor.hide()
		tabDisplay().get_node("Panel").add_child(gameEditor)
		gameEditor.setEditable(details.gameData.hostID == GameData.id and details.gameData.startTime > Time.get_unix_time_from_system() + 2 * 365 * 24 * 60 * 60)
		gameEditor.init(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var details = GameData.getGameDetails(gameID)
	
	if get_viewport().size.x < get_viewport().size.y:
		if $Viewport/GameOverlay/VMenuBar.get_child_count() > 0:
			get_node("Viewport/GameOverlay/VMenuBar/Tabs/HBoxContainer/ChatContainer/ChatNotification").visible = details.gameData.hasChatNotifications
			get_node("Viewport/GameOverlay/VMenuBar/Tabs/HBoxContainer/ShopContainer/ShopNotification").visible = game.getHires() > 0
	else:
		if $Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.get_child_count() > 0:
			get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar/Tabs/VBoxContainer/ChatContainer/ChatNotification").visible = details.gameData.hasChatNotifications
			get_node("Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar/Tabs/VBoxContainer/ShopContainer/ShopNotification").visible = game.getHires() > 0
	
	var message = game.getNextVictoryMessage()
	
	if message != "" and not (message.contains("has won") and GameData.isFinished(gameID)):
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/VictoryMessage.show()
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/VictoryMessage.text = message
	else:
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/VictoryMessage.hide()
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/VictoryMessage.text = ""
	
	if not game.hasStarted() or game.isPaused():
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted.show()
		if game.isPaused():
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted/Label.text = "Game is paused"
		elif (game.getStartTime() - game.getTime()) < (2 * 365 * 24 * 60 * 60): #if less than 2 years
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted/Label.text = "Game starts in " + Utilities.timeToStr(game.getStartTime() - game.getTime())
		elif details.gameData.playerCount < details.gameSettings.playerCap:
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted/Label.text = "Waiting for players (" + str(details.gameData.playerCount) + "/" + str(details.gameSettings.playerCap) + ")"
		else: 
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted/Label.text = "Waiting for host to finalize settings..."
	else:
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/NotStarted.hide()

	if game.getSelectedUnits() >= 0:
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/Label.text = str(floor(game.getSelectedUnits() * $Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/HBoxContainer/Control/PercentBar.value))
	else:
		$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/VBoxContainer/HBoxContainer/Control/MarginContainer/MarginContainer/Label.text = ""
	
	if not game.isOffline():
		if game.hasLost():
			if not hasLost:
				hasLost = true
				
				await GameData.viewEnd(gameID)
		
		if game.hasEnded() or GameData.isFinished(gameID):
			if not viewingEnd:
				viewingEnd = true
				await GameData.viewEnd(gameID)
				
				if not game.hasEnded():
					game.startAtBeginning()
		
				if not GameData.isFinished(gameID):
					if await GameData.verifyEnd(gameID):
						$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/EndGame.init(gameID)
						$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/EndGame.show()
				else:
					$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/EndGame.init(gameID)
					$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/EndGame.show()
		else:
			viewingEnd = false
			$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/EndGame.hide()
		
		if details.gameData.hostID == GameData.id and details.gameData.startTime > Time.get_unix_time_from_system() + 2 * 365 * 24 * 60 * 60:
			tabDisplay().get_node("Panel/GameEditor").setEditable(true)
			tabDisplay().get_node("Panel/GameEditor").setActivatable(true)
		else:
			tabDisplay().get_node("Panel/GameEditor").setEditable(false)
			tabDisplay().get_node("Panel/GameEditor").setActivatable(false)
	
	var rect = $Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay.get_global_rect()
	
	var s = get_viewport().get_visible_rect().size.y * 1.0 / DisplayServer.screen_get_size(DisplayServer.window_get_current_screen()).y
	
	var screenY = DisplayServer.screen_get_size(DisplayServer.window_get_current_screen()).y * s
	
	
	var keyboardTop = screenY - (DisplayServer.virtual_keyboard_get_height() if DisplayServer.has_feature(DisplayServer.FEATURE_VIRTUAL_KEYBOARD) else 0) * s
	
	if keyboardTop < rect.position.y + rect.size.y - 75:
		diff = 0.5 * diff + 0.5 * (rect.position.y + rect.size.y - 75 - keyboardTop)
	elif abs(diff) > 0.01:
		diff *= 0.5
	else:
		diff = 0
	
	$Viewport/GameOverlay/MarginContainer/HBoxContainer/Overlay/UIOverlay.add_theme_constant_override("margin_bottom", 150 + diff)
	
func addOrder(type, referenceID, timestamp, arguments):
	GameData.addOrder(gameID, type, referenceID, timestamp, arguments)

func replaceOrder(id, type, referenceID, canceled, timestamp, arguments, oldArguments):
	GameData.replaceOrder(id, gameID, type, referenceID, canceled, timestamp, arguments, oldArguments)

func setDisplay(scene):
	if detailDisplay != null:
		elementDisplay().get_node("Panel/VBoxContainer/MarginContainer").remove_child(detailDisplay)
	
	detailDisplay = scene

	elementDisplay().get_node("Panel/VBoxContainer/MarginContainer").add_child(detailDisplay)
	elementDisplay().get_node("Panel").show()

func selectVessel(vessel):
	var scene = preload("res://VesselDetails.tscn").instantiate()
	scene.init(vessel, gameID)
	scene.battleForecastToggle.connect(battleForecast)
	$Viewport/Viewport3D/CameraManager.selected(vessel)
	setDisplay(scene)

func battleForecast(node):
	if tabDisplay().get_node("Panel/Battle Forecast").visible:
		setMenuDisplay(null, false)
	else:
		var scene = tabDisplay().get_node("Panel/Battle Forecast")
		scene.queue_free()
		tabDisplay().get_node("Panel").remove_child(scene)
		
		scene = preload("res://Battle Forecast.tscn").instantiate()
		scene.init(gameID, node.getID())
		tabDisplay().get_node("Panel").add_child(scene)
		setMenuDisplay(scene, true)
	

func selectOutpost(outpost):
	print("Debug: got to the selectOutpost() function")
	var scene = preload("res://OutpostDetails.tscn").instantiate()
	scene.init(outpost, gameID)
	scene.battleForecastToggle.connect(battleForecast)
	$Viewport/Viewport3D/CameraManager.selected(outpost)
	setDisplay(scene)
	print("Debug: finished the selectOutpost() function")
	
func selectSpecialist(specialist):
	var scene = preload("res://SpecialistDetails.tscn").instantiate()
	scene.init(specialist, gameID)
	scene.openShop.connect(_on_shop_button_pressed)
	setDisplay(scene)
	
func deselect():
	if(detailDisplay):
		elementDisplay().get_node("Panel/VBoxContainer/MarginContainer").remove_child(detailDisplay)
		detailDisplay= null
		elementDisplay().get_node("Panel").hide()

func deselectSpecialist(specialist):
	if detailDisplay and "specialistID" in detailDisplay and detailDisplay.specialistID == specialist:
		elementDisplay().get_node("Panel/VBoxContainer/MarginContainer").remove_child(detailDisplay)
		detailDisplay= null
		elementDisplay().get_node("Panel").hide()

func _on_percent_bar_value_changed(value):
	game.setPercent(value)

func setMenuDisplay(scene, select):
	if scene == menuDisplay:
		scene = null
		select = false
	
	if scene and menuDisplay:
		menuDisplay.hide()
		menuDisplay = null

	if scene == null or not select:
		tabDisplay().get_node("AnimationPlayer").play("slide_down")
	else:
		menuDisplay = scene
		menuDisplay.show()
		
		if not tabDisplay().get_node("Panel").visible or tabDisplay().get_node("Panel").modulate.a < 1.0:
			tabDisplay().get_node("Panel").show()
			tabDisplay().get_node("AnimationPlayer").play("slide_up")

func _on_status_button_pressed():
	setMenuDisplay(tabDisplay().get_node("Panel/Status"), true)

func _on_chat_button_pressed():
	setMenuDisplay(tabDisplay().get_node("Panel/Chat"), true)

func _on_shop_button_pressed():
	setMenuDisplay(tabDisplay().get_node("Panel/Shop"), true)

func _on_editor_button_pressed():
	setMenuDisplay(tabDisplay().get_node("Panel/GameEditor"), true)

func _on_order_button_pressed():
	setMenuDisplay(tabDisplay().get_node("Panel/Orders"), true)
	
func _on_back_button_pressed():
	GameData.previous()

func _on_camera_manager_unselect():
	setMenuDisplay(null, false)
	
	if get_viewport().size.x > get_viewport().size.y:
		if $Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.get_child_count() > 0:
			var tabs = $Viewport/GameOverlay/MarginContainer/HBoxContainer/HMenuBar.get_child(0)
			
			for child in tabs.get_node("VBoxContainer").get_children():
				if child.get_child_count() > 0:
					child.get_child(0).button_pressed = false
	else:
		if $Viewport/GameOverlay/VMenuBar.get_child_count() > 0:
			var tabs = $Viewport/GameOverlay/VMenuBar.get_child(0)
			
			for child in tabs.get_node("HBoxContainer").get_children():
				if child.get_child_count() > 0:
					child.get_child(0).button_pressed = false
	
func _exit_tree():
	$Viewport/Viewport3D/CameraManager/FloorDisplay.remove_child(game.getFloorDisplay())
	game.getFloorDisplay().set_process(false)
	$Viewport/Viewport3D.remove_child(game)
	
	game.set_process(false)
	game.set_visible(false)
	game.suspend()
		
func _on_animation_player_animation_finished(anim_name):
	if anim_name == "slide_down":
		tabDisplay().get_node("Panel").hide()
		
		if menuDisplay:
			menuDisplay.hide()
			menuDisplay = null


func _on_shop_deselect_shop():
	_on_camera_manager_unselect()

func _on_confirm_surrender_pressed():
	GameData.addOrder(gameID, "SURRENDER", int(game.getReferenceID()), game.getTime(), [])
	$Viewport/GameOverlay/MarginContainer/SurrenderDialog.hide()

func _on_cancel_surrender_pressed():
	$Viewport/GameOverlay/MarginContainer/SurrenderDialog.hide()

func _on_status_surrender():
	$Viewport/GameOverlay/MarginContainer/SurrenderDialog.show()
