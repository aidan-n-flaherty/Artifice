extends MarginContainer
class_name GameEditor

var advancedSettings = ["defaultSonar", "defaultMaxShield", "resourcesToWin", "factoryDensity", "fireRate", "fireRange", "costPerMine", "outpostsPerPlayer"]

var changedAdvancedSettings = []

var days = []

var activeHours = range(0, 24)

var simulationTimescale = "days"

var numPlayers = 10

var editable = false

var gameID = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.gameChanged.connect(update)
	
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.get_children():
		child.toggled.connect(on_players_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.get_children():
		child.toggled.connect(on_timescale_modified.bind(child.name.to_lower()))
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons.get_children():
		child.toggled.connect(on_activeTimes_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2.get_children():
		child.toggled.connect(on_activeTimes_modified)

func setEditable(canEdit):
	editable = canEdit
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Editable.visible = not canEdit
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Editable.visible = not canEdit
	
	if editable:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/System.hide()
		$MarginContainer/VBoxContainer/Passworded.hide()
		$MarginContainer/VBoxContainer/Players.hide()
	else:
		$MarginContainer/VBoxContainer/Activate.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.hide()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func update(gameID):
	if gameID == self.gameID:
		deserialize(self.gameID)

func deserialize(gameID):
	print("deserializing...")
	
	self.gameID = gameID
	
	var details = GameData.getGameDetails(gameID)
	
	print(details)
	
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.text = details.gameSettings.lobbyName
	
	var data = details.gameData
	#version
	#creation date
	var settings = details.gameSettings
	
	var date = data["createdAt"]
	var stringDate = Utilities.timeToDateStr(int(date))
	
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/System/Grid/CreationDateText.text = stringDate
	
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/System/Grid/VersionText.text = str(data.version)
	
	$MarginContainer/VBoxContainer/Passworded.visible = data.hasPassword
	
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value = settings.minRating
		
	var numCurrentPlayers = data["playerCount"]
	numPlayers = settings["playerCap"]
	
	get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/" + str(numPlayers)).button_pressed = true
	
	var simSp = SettingsDefault.getSimulationSpeed()
	if(settings.settingOverrides.has("simulationSpeed")):
		simSp = settings.settingOverrides["simulationSpeed"]
	
	simulationTimescale = SettingsDefault.getSimulationTimescale()
	simulationTimescale = simulationTimescale.to_lower()
	
	simulationTimescale = "days" if simSp <= 2.0 and simSp >= 0.5 else "hours" if (simSp <= 120.0 and simSp >= 30.0) else "minutes" if (simSp <= 120.0*60 and simSp >= 30.0*60) else "days"
	
	simSp /= 1 if simulationTimescale == "days" else 60 if simulationTimescale == "hours" else 60*60
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value = simSp

	
	if (simulationTimescale != "days"):
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.visible = false
	
	get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons/" + simulationTimescale).button_pressed = true
	
	if (settings.settingOverrides.has("activeHours")):
		activeHours = settings.settingOverrides["activeHours"]
	else:
		activeHours = range(24)
	
	for hour in range(24):
		var aButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons/" if hour < 12 else "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2/"
		aButtonPath += str(hour)
		var buttonAH = get_node(aButtonPath)
		buttonAH.button_pressed = hour in activeHours
	
	var hasAdvanced = false
		
	for setting in advancedSettings:
		var default = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting)
		var defaultText = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting + "Text")
		
		if not default or not defaultText:
			continue
		
		if settings.settingOverrides.has(setting):
			var value = settings.settingOverrides[setting]
			
			if value != default.value:
				default.value = value
				default.show()
				defaultText.show()
				
				hasAdvanced = true
		else:
			default.hide()
			defaultText.hide()
		
	if hasAdvanced or editable:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/AdvancedButton.show()
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/AdvancedButton.hide()
		
	var playerViews = []
	
	for	i in range(int(settings.playerCap)):
		var playerView = preload("res://Game_PlayerView.tscn").instantiate()
		playerViews.append(playerView)
		$MarginContainer/VBoxContainer/Players/MarginContainer/PlayerList/GridContainer.add_child(playerView)
		
	var users = await GameData.getGameUsers(gameID)
	var userKeys = Array(users.keys())
	
	var colors = SettingsDefault.getPlayerColors()
	
	var playerView
	
	for	i in userKeys:
		var id = int(i)
		var color = colors[id]
		var username = users[i].username
		
		playerViews[id].init(username, color)

func serialize():
	var bias = int(Time.get_time_zone_from_system().bias/60)
	
	var hours = []
	
	for hour in activeHours:
		hour += bias
		
		while hour >= 24:
			hour -= 24
		while hour < 0:
			hour += 24
		
		hours.push_back(hour)
	
	hours.sort()

	var data = {
		"lobbyName": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.text,
		"password": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.text,
		"playerCap": numPlayers,
		"startTimeDisplacement": 24 * 60 * 60 if simulationTimescale == "days" else 60 * 10 if simulationTimescale == "hours" else 30,
		"settingOverrides": {
			"ratingConstraints": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value,
			"activeHours": hours,
			"simulationSpeed": (1 if simulationTimescale == "days" else 60 if simulationTimescale == "hours" else 60 * 60) * $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value,
			"gameMode": "CONQUEST" if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/conquest.button_pressed else "KOH",
		}
	}
	
	for setting in advancedSettings:
		var slider = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting)
		
		if not slider:
			continue
		
		data.settingOverrides[setting] = slider.value
	
	return data

func activate():
	var game = await HTTPManager.postReq("/createMatch", serialize(), {})
	
	if game:
		print("Created game")
		print(game)
		GameData.addGame(game)
		
		return game
	
	print("Failed to create game")
	return null

func on_players_modified(button_pressed: bool):
	if button_pressed:
		for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.get_children():
			if child.button_pressed:
				numPlayers = child.name.to_int()

func on_activeTimes_modified(button_pressed: bool):
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons.get_children():
		if child.button_pressed:
			if child.name.to_int() in activeHours:
				activeHours.erase(child.name.to_int())
			else:
				activeHours.append(child.name.to_int())
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2.get_children():
		if child.button_pressed:
			if child.name.to_int() in activeHours:
				activeHours.erase(child.name.to_int())
			else:
				activeHours.append(child.name.to_int())

func on_timescale_modified(button_pressed: bool, timescale):
	simulationTimescale = timescale
	
	if button_pressed:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.visible = simulationTimescale == "days"
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.visible = simulationTimescale == "days"

func _on_create_toggled(toggled_on):
	var game = await serialize()
	
	if game:
		GameData.goto_scene("res://OpenGameList.tscn")


func _on_advanced_button_pressed():
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible = not $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible
