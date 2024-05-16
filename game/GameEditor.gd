extends MarginContainer
class_name GameEditor

var advancedSettings = ["defaultSonar", "defaultMaxShield", "resourcesToWin", "factoryDensity", "fireRate", "fireRange", "costPerMine", "outpostsPerPlayer"]

var advancedSettingsDefaults = {}

var changedAdvancedSettings = []

var days = []

var activeHours = range(0, 24)

var simulationTimescale = "days"

var numPlayers = 10

var editable = false

var gameID = -1

var number_of_teams = 0

var button_team_num = 0

var team_button_pressed = false


# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.gameChanged.connect(update)
	
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer.get_children():
		child.toggled.connect(on_players_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.get_children():
		child.toggled.connect(on_timescale_modified.bind(child.name.to_lower()))
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons.get_children():
		child.toggled.connect(on_activeTimes_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons2.get_children():
		child.toggled.connect(on_activeTimes_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.get_children():
		child.toggled.connect(on_team_number_modified)

func setEditable(canEdit):
	editable = canEdit
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Editable.visible = not canEdit
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Editable.visible = not canEdit
	
	if editable:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/System.hide()
		$MarginContainer/VBoxContainer/Passworded.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.show()
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/System.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.hide()
	
	if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_child_count() == 0:
		for specialist in SettingsDefault.getAllSpecialists():
			var specialistName = SettingsDefault.getSpecialistName(specialist)
			
			var button = Button.new()
			button.name = str(specialist)
			button.text = specialistName
			
			button.toggle_mode = true
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.add_child(button)

func playersVisible(value: bool):
	$MarginContainer/VBoxContainer/Players.visible = value

func setActivatable(activatable: bool):
	$MarginContainer/VBoxContainer/Activate.visible = activatable

func getActivationButton():
	return $MarginContainer/VBoxContainer/Activate

func getSecondaryButton():
	return $MarginContainer/VBoxContainer/Secondary

func setFinalizing(value: bool):
	$MarginContainer/VBoxContainer/Finalizing.visible = value

func setFinalizeText(text):
	$MarginContainer/VBoxContainer/Finalizing/MarginContainer/Finalize.text = text

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func update(gameID):
	if gameID == self.gameID:
		deserialize(self.gameID)

func deserialize(gameID):
	if self.gameID == -1:
			for setting in advancedSettings:
				var default = get_node_or_null("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting)
				
				if default:
					advancedSettingsDefaults[setting] = default.value
	
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
		
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons/ranked.button_pressed = details.gameSettings.ranked
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons/unranked.button_pressed = not details.gameSettings.ranked
	
	if settings.settingOverrides.has("gameMode"):
		if settings.settingOverrides["gameMode"] == "CONQUEST":
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/Conquest.button_pressed = true
		elif settings.settingOverrides["gameMode"] == "ELIMINATION":
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/Elimination.button_pressed = true
		else:
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/Mining.button_pressed = true
			
	
	var numCurrentPlayers = data["playerCount"]
	numPlayers = settings["playerCap"]
	
	for i in range(2, numCurrentPlayers):
		get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer/" + str(i)).disabled = true
	
	get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer/" + str(numPlayers)).button_pressed = true
	on_players_modified(true)
	
	var simSp = SettingsDefault.getSimulationSpeed()
	print("Settings default", simSp)
	if(settings.settingOverrides.has("simulationSpeed")):
		simSp = settings.settingOverrides["simulationSpeed"]
	
	simulationTimescale = "days" if simSp <= 2.0 and simSp >= 0.5 else "hours" if (simSp <= 120.0 and simSp >= 30.0) else "minutes" if (simSp <= 120.0*60 and simSp >= 30.0*60) else "days"
	
	simSp /= 1 if simulationTimescale == "days" else 60 if simulationTimescale == "hours" else 60*60
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value = simSp

	
	if (simulationTimescale != "days"):
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.visible = false
	
	get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons/" + simulationTimescale).button_pressed = true
	
	if settings.settingOverrides.has("number_of_teams"):
		number_of_teams = int(settings.settingOverrides["number_of_teams"])
		
		if number_of_teams > 1 and get_node_or_null("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/" + str(number_of_teams)):
			get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/" + str(number_of_teams)).button_pressed = true
	
	var bias = int(Time.get_time_zone_from_system().bias/60)

	if (settings.settingOverrides.has("activeHours")):
		print("a ", settings.settingOverrides["activeHours"])
		activeHours = []
		for hour in settings.settingOverrides["activeHours"]:
			hour = int(hour)
			hour += bias
		
			while hour >= 24:
				hour -= 24
			while hour < 0:
				hour += 24
			activeHours.push_back(hour)
		print("b ", activeHours)
		activeHours.sort()
	else:
		activeHours = range(24)
	
	for hour in range(24):
		var aButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons/" if hour < 12 else "MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons2/"
		aButtonPath += str(hour)
		var buttonAH = get_node(aButtonPath)
		buttonAH.set_pressed_no_signal(hour in activeHours)
	
	var hasAdvanced = false
	
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_children():
		child.button_pressed = false
	
	if settings.settingOverrides.has("specialistBans") and len(settings.settingOverrides["specialistBans"]) > 0:
		for ban in settings.settingOverrides["specialistBans"]:
			for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_children():
				if child.name == str(ban):
					hasAdvanced = true
					child.button_pressed = true
					break
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBans.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons.show()
		
		for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_children():
			var found = false
			
			for ban in settings.settingOverrides["specialistBans"]:
				if child.name == str(ban):
					found = true
					break
			
			if not found:
				$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.move_child(child, $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_child_count() - 1)
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBans.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons.hide()
		
	for setting in advancedSettings:
		var default = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting)
		var defaultText = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting + "Text")
		
		if not default or not defaultText:
			continue
		
		if settings.settingOverrides.has(setting):
			var value = settings.settingOverrides[setting]
			
			if value != advancedSettingsDefaults[setting]:
				default.value = value
				default.show()
				defaultText.show()
				
				hasAdvanced = true
			elif not editable:
				default.hide()
				defaultText.hide()
		elif not editable:
			default.hide()
			defaultText.hide()
		
	if hasAdvanced or editable:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/AdvancedButton.show()
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/AdvancedButton.hide()
	
	await updatePlayers(gameID, settings)
	
	print("Loaded game editor")

func updatePlayers(gameID, settings):
	var playerViews = []
	
	for n in $MarginContainer/VBoxContainer/Players/MarginContainer/PlayerList/GridContainer.get_children():
		$MarginContainer/VBoxContainer/Players/MarginContainer/PlayerList/GridContainer.remove_child(n)
		n.queue_free() 
	
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
		
		playerViews[id].init(users[i].id, users[i].username, color, users[i].userStats.rating, settings.ranked)

func serialize():
	var bias = int(Time.get_time_zone_from_system().bias/60)
	
	var hours = []
	
	print("c ", activeHours)
	
	for hour in activeHours:
		hour -= bias
		
		while hour >= 24:
			hour -= 24
		while hour < 0:
			hour += 24
		
		hours.push_back(hour)
		
	
	hours.sort()
	print("d ", hours)
	
	##Checks to make sure that the current team count is valid. If it isn't automatically sets
	##number of teams to zero
	if number_of_teams != 0 and numPlayers % number_of_teams != 0:
		number_of_teams = 0
	
	print("creating game")
	var data = {
		"lobbyName": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.text,
		"password": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.text,
		"playerCap": numPlayers,
		"ranked": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons/ranked.button_pressed,
		"startTimeDisplacement": 24 * 60 * 60 if simulationTimescale == "days" else 60 * 10 if simulationTimescale == "hours" else 30,
		"minRating": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value,
		"settingOverrides": {
			"simulationSpeed": (1 if simulationTimescale == "days" else 60 if simulationTimescale == "hours" else 60 * 60) * $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SpeedSlider.value,
			"number_of_teams": number_of_teams,
			"ratingConstraints": $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.value,
			"activeHours": hours,
			"gameMode": "CONQUEST" if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/Conquest.button_pressed else "ELIMINATION" if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons/Elimination.button_pressed else "MINING",
		}
	}
	
	var bans = []
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/SpecialistBansButtons/SpecialistBansButtons.get_children():
		if child.button_pressed:
			bans.push_back(child.name.to_int())
	
	if len(bans) > 0:
		data.settingOverrides["specialistBans"] = bans
	
	for setting in advancedSettings:
		var slider = get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced/Grid/" + setting)
		
		if not slider:
			continue
		
		data.settingOverrides[setting] = slider.value
	
	return data

func activate():
	pass

func createGame():
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
		for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer.get_children():
			if child.button_pressed:
				numPlayers = int(str(child.name))
				for team_num in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.get_children():
					button_team_num = int(str(team_num.name))
					team_button_pressed = int(str(team_num.name))
					if(button_team_num == 0):
						team_num.visible = true
					elif( ((numPlayers % button_team_num) == 0) && !(numPlayers == button_team_num)):
						team_num.visible = true
					else:
						team_num.visible = false

				if( (number_of_teams > 0) && ((numPlayers % number_of_teams != 0) || (numPlayers == number_of_teams)) ): 
					$"MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/0".set_pressed_no_signal(true)
					get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer/" + str(numPlayers)).set_pressed_no_signal(false)
				elif( (number_of_teams > 0) && (numPlayers % number_of_teams == 0) && (numPlayers != number_of_teams)):
					$"MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/0".set_pressed_no_signal(false)
					get_node("MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons/HBoxContainer/" + str(numPlayers)).set_pressed_no_signal(true)

func on_activeTimes_modified(button_pressed: bool):
	activeHours.clear()
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons.get_children():
		if child.button_pressed:
			activeHours.append(child.name.to_int())
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows/VBoxContainer/ActiveHoursButtons2.get_children():
		if child.button_pressed:
			activeHours.append(child.name.to_int())
	activeHours.sort()
	print("e", activeHours)

func on_timescale_modified(button_pressed: bool, timescale):
	simulationTimescale = timescale
	
	if button_pressed:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.visible = simulationTimescale == "days"
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.visible = simulationTimescale == "days"
		
func on_team_number_modified(button_pressed: bool):
	for child in $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.get_children():
		if child.button_pressed:
			number_of_teams = int(str(child.name))


func _on_advanced_button_pressed():
	$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible = not $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Advanced.visible
