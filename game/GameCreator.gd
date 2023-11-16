extends MarginContainer

var days = []

var activeHours = []

var simulationTimescale = "days"

var numPlayers = 10

# Called when the node enters the scene tree for the first time.
func _ready():
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/NumPlayersButtons.get_children():
		child.toggled.connect(on_players_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons.get_children():
		child.toggled.connect(on_activeTimes_modified)
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons2.get_children():
		child.toggled.connect(on_activeTimes_modified)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_create_pressed():
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
		"lobbyName": $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/LobbyNameText.text,
		"password": $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/PasswordText.text,
		"playerCap": numPlayers,
		"startTimeDisplacement": 24 * 60 * 60 if simulationTimescale == "day" else 60 * 10 if simulationTimescale == "minute" else 30,
		"settingOverrides": {
			"ratingConstraints": $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/RatingSlider.value,
			"activeHours": hours,
			"simulationTimescale": simulationTimescale,
			"simulationSpeed": (1 if simulationTimescale == "day" else 60 if simulationTimescale == "minute" else 60 * 60) * $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/SpeedSlider.value
		}
	}
	
	GameData.addGame(await HTTPManager.postReq("/createMatch", data, {}))

func on_players_modified():
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/NumPlayersButtons.get_children():
		if child.pressed:
			numPlayers = int(str(child.name))

func on_activeTimes_modified(button_pressed: bool):
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons.get_children():
		if child.pressed:
			numPlayers = int(str(child.name))
	for child in $MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows/ActiveHoursButtons2.get_children():
		if child.pressed:
			numPlayers = int(str(child.name))

func _on_days_toggled(button_pressed):
	if button_pressed:
		simulationTimescale = "days"
		$MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHours.show()
		$MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows.show()
	else:
		$MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHours.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/GridContainer/ActiveHoursRows.hide()


func _on_hours_toggled(button_pressed):
	if button_pressed:
		simulationTimescale = "hours"


func _on_minutes_toggled(button_pressed):
	if button_pressed:
		simulationTimescale = "minutes"
