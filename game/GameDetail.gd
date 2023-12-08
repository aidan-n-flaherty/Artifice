extends MarginContainer

var gameID
# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID):
	self.gameID = gameID
	
	var details = GameData.getGameDetails(gameID) 
	
	$MarginContainer/VBoxContainer/Title.text = details.gameSettings.lobbyName
	
	#password
	
	var data = details.gameData
	#version
	#creation date
	var settings = details.gameSettings
	
	$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/VersionText.text = str(data["version"])
	
	var date = data["createdAt"]
	var stringDate = Time.get_date_string_from_unix_time(date)
	
	$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/CreationDate.text = stringDate
	
	var numCurrentPlayers = data["playerCount"]
	var numPlayers = settings["playerCap"]
	
	var pButtonPath= "MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/NumPlayersButtons/"
	pButtonPath += str(numPlayers)
	
	
	var buttonPlayer = get_node(pButtonPath)
	buttonPlayer.disabled = false
	
	var simSp = SettingsDefault.getSimulationSpeed()
	if(settings.settingOverrides.has("simulationSpeed")):
		simSp = settings.settingOverrides["simulationSpeed"]
	
	
	var simT = SettingsDefault.getSimulationTimescale()
	simT = simT.to_lower()
	
	if(simSp <= 2.0 and simSp >= 0.5):
		simT = "days"
	elif (simSp <= 120.0 and simSp >= 30.0):
		simT = "hours"
	elif (simSp <= 120.0*60 and simSp >= 30.0*60):
		simT = "minutes"
	
	if (simT == "hours"):
		simSp /= 60
	elif (simT == "minutes"):
		simSp /= 60*60
	$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/SpeedSlider.value = simSp
	

	
	if (simT != "days"):
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/ActiveHours.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/ActiveHoursRows.visible = false
	
	var sButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/SimulationTimescaleButtons/"
	sButtonPath += simT
	
	var buttonSim = get_node(sButtonPath)
	buttonSim.disabled = false
	
	var activeHours
	if (settings.settingOverrides.has("activeHours")):
		activeHours = settings.settingOverrides["activeHours"]
	else:
		activeHours = range(24)
	
	for hour in activeHours:
		var aButtonPath
		if (hour < 12):
			aButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons/"
		else:
			aButtonPath = "MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons2/"
		aButtonPath += str(hour)
		var buttonAH = get_node(aButtonPath)
		buttonAH.disabled = false
	
	var ratings
	if settings.settingOverrides.has("ratingConstraints"):
		ratings = settings.settingOverrides["ratingConstraints"]
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/RatingSlider.value = ratings
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/Rating.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/RatingSlider.visible = false
	#rating should not be in settingOverrides
	
	var users = await GameData.getGameUsers(gameID)
	var userKeys = Array(users.keys())
	
	var colors = SettingsDefault.getPlayerColors()
	
	var playerView
	
	for	i in userKeys:
		var user = int(i)
		var color = colors[user]
		var username = users[i].username
		
		playerView = preload("res://Game_PlayerView.tscn").instantiate()
		playerView.init(username, color)
		$MarginContainer/VBoxContainer/Players/MarginContainer/PlayerList/GridContainer.add_child(playerView)
	
	var remaining = numPlayers - numCurrentPlayers
	
	for i in remaining:
		var color = Color(0,0,0)
		var username = "Open Slot"
		
		playerView = preload("res://Game_PlayerView.tscn").instantiate()
		playerView.init(username, color)
		$MarginContainer/VBoxContainer/Players/MarginContainer/PlayerList/GridContainer.add_child(playerView)
	
	if !data.hasPassword:
		$MarginContainer/VBoxContainer/Passworded.visible = false
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_back_button_pressed():
	GameData.goto_scene("res://MainMenu.tscn")


func _on_button_pressed():
	if await GameData.joinGame(gameID, $MarginContainer/VBoxContainer/Passworded/PasswordText.text):
		GameData.goto_scene("res://MainMenu.tscn")
