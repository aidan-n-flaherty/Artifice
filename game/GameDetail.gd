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
	
	var simT = SettingsDefault.getSimulationTimescale()
	
	if (settings.settingOverrides.has("simulationTimescale")):
		simT = settings.settingOverrides["simulationTimescale"]
	simT = simT.to_lower()
	
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
	
		
	var simSp = SettingsDefault.getSimulationSpeed()
	if(settings.settingOverrides.has("simulationSpeed")):
		simSp = settings.settingOverrides["simulationSpeed"]
	
	$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/SpeedSlider.value = simSp
	
	var ratings
	if settings.settingOverrides.has("ratingConstraints"):
		ratings = settings.settingOverrides["ratingConstraints"]
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/RatingSlider.value = ratings
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/Rating.visible = false
		$MarginContainer/VBoxContainer/ScrollContainer/MarginContainer/GridContainer/RatingSlider.visible = false
	#should not be in settingOverrides
	
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_back_button_pressed():
	GameData.goto_scene("res://MainMenu.tscn")


func _on_button_pressed():
	if await GameData.joinGame(gameID):
		GameData.goto_scene("res://MainMenu.tscn")
