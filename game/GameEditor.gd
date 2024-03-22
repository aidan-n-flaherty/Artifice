extends MarginContainer
class_name GameEditor

var days = []

var activeHours = range(0, 24)

var simulationTimescale = "days"

var numPlayers = 10

var editable = false

var gameID = -1

# Called when the node enters the scene tree for the first time.
func _ready():
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
		$MarginContainer/VBoxContainer/Create.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.hide()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func deserialize(gameID):
	self.gameID = gameID
	
	var details = GameData.getGameDetails(gameID)
	
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
	
	var numCurrentPlayers = data["playerCount"]
	var numPlayers = settings["playerCap"]
	
	var pButtonPath= "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/"
	pButtonPath += str(numPlayers)
	
	var buttonPlayer = get_node(pButtonPath)
	buttonPlayer.disabled = false
	
	var simSp = SettingsDefault.getSimulationSpeed()
	if(settings.settingOverrides.has("simulationSpeed")):
		simSp = settings.settingOverrides["simulationSpeed"]
	
	var simT = SettingsDefault.getSimulationTimescale()
	simT = simT.to_lower()
	
	simT = "days" if simSp <= 2.0 and simSp >= 0.5 else "hours" if (simSp <= 120.0 and simSp >= 30.0) else "minutes" if (simSp <= 120.0*60 and simSp >= 30.0*60) else "days"
	
	simSp /= 1 if simT == "days" else 60 if simT == "hours" else 60*60
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value = simSp

	
	if (simT != "days"):
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.visible = false
	
	var sButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons/"
	sButtonPath += simT
	
	var buttonSim = get_node(sButtonPath)
	buttonSim.disabled = false
	
	var activeHours
	if (settings.settingOverrides.has("activeHours")):
		activeHours = settings.settingOverrides["activeHours"]
	else:
		activeHours = range(24)
	
	for hour in range(24):
		var aButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons/" if hour < 12 else "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2/"
		aButtonPath += str(hour)
		var buttonAH = get_node(aButtonPath)
		buttonAH.disabled = hour not in activeHours
	
	var ratings
	if settings.settingOverrides.has("ratingConstraints"):
		ratings = settings.settingOverrides["ratingConstraints"]
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value = ratings
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Rating.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.visible = false
	#rating should not be in settingOverrides
	
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
	print("creating game")
	var data = {
		"lobbyName": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.text,
		"password": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.text,
		"playerCap": numPlayers,
		"startTimeDisplacement": 24 * 60 * 60 if simulationTimescale == "days" else 60 * 10 if simulationTimescale == "hours" else 30,
		"settingOverrides": {
			"ratingConstraints": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value,
			"activeHours": hours,
			"simulationSpeed": (1 if simulationTimescale == "days" else 60 if simulationTimescale == "hours" else 60 * 60) * $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value,
			"factoryDensity": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/FactorySlider.value,
			"resourcesToWin": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/VictoryAmountSlider.value,
			"gameMode": "CONQUEST" if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/conquest.button_pressed else "KOH",
			"defaultSonar": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SonarSlider.value,
			"defaultMaxShield": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/BaseShieldSlider.value,
			"fireRate": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/FireRateSlider.value,
			"fireRange": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/FireRangeSlider.value,
			"costPerMine": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/MineCostSlider.value,
			"outpostsPerPlayer": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/OutpostCountSlider.value,
		}
	}
	
	var game = await HTTPManager.postReq("/createMatch", data, {})
	print(game)
	GameData.addGame(game)
	
	return game

func on_players_modified(button_pressed: bool):
	if button_pressed:
		for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.get_children():
			if child.button_pressed:
				numPlayers = int(str(child.name))

func on_activeTimes_modified(button_pressed: bool):
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons.get_children():
		if child.pressed:
			if int(child.name) in activeHours:
				activeHours.erase(int(child.name))
			else:
				activeHours.append(int(child.name))
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2.get_children():
		if child.pressed:
			if int(child.name) in activeHours:
				activeHours.erase(int(child.name))
			else:
				activeHours.append(int(child.name))

func on_timescale_modified(button_pressed: bool, timescale):
	simulationTimescale = timescale
	
	if button_pressed:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons.visible = simulationTimescale == "days"
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/ActiveHoursButtons2.visible = simulationTimescale ==" days"

func _on_create_toggled(toggled_on):
	var game = await serialize()
	
	if game:
		GameData.goto_scene("res://OpenGameList.tscn")


func _on_advanced_button_pressed():
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible = not $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible
