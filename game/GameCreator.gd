extends Control

var days = []

var simulationTimescale

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_create_pressed():
	var bias = 60 * Time.get_time_zone_from_system().bias
	
	var utcDays = days.duplicate()
	
	var simulationStartTime = bias + 60 * 60 * $MarginContainer/VBoxContainer/StartTime/HSlider.value
	if simulationStartTime >= 24 * 60 * 60:
		simulationStartTime -= 24 * 60 * 60
		utcDays.map(func(day): day = 6 if day == 0 else day - 1)
	
	var duration = 60 * 60 * $MarginContainer/VBoxContainer/Duration/HSlider.value
	
	var data = {
		"lobbyName": $MarginContainer/VBoxContainer/LobbyName/Input.text,
		"password": $MarginContainer/VBoxContainer/Password/Input.text,
		"playerCap": $MarginContainer/VBoxContainer/NumPlayers/HSlider.value,
		"startTimeDisplacement": 24 * 60 * 60 if simulationTimescale == "day" else 60 * 10 if simulationTimescale == "minute" else 30,
		"settingOverrides": {
			"ratingConstraints": $MarginContainer/VBoxContainer/RatingConstraints/HSlider.value,
			"days": utcDays,
			"activeTime": [simulationStartTime, simulationStartTime + duration],
			"simulationTimescale": simulationTimescale,
			"simulationSpeed": $MarginContainer/VBoxContainer/SimulationSpeed/HSlider.value
		}
	}
	
	GameData.addGame(await HTTPManager.postReq("/createMatch", data, {}))

func _on_sunday_toggled(button_pressed):
	if button_pressed:
		days.push_back(0)
	else:
		days = days.filter(func(day): day == 0)


func _on_monday_toggled(button_pressed):
	if button_pressed:
		days.push_back(1)
	else:
		days = days.filter(func(day): day == 1)


func _on_tuesday_toggled(button_pressed):
	if button_pressed:
		days.push_back(2)
	else:
		days = days.filter(func(day): day == 2)


func _on_wednesday_toggled(button_pressed):
	if button_pressed:
		days.push_back(3)
	else:
		days = days.filter(func(day): day == 3)


func _on_thursday_toggled(button_pressed):
	if button_pressed:
		days.push_back(4)
	else:
		days = days.filter(func(day): day == 4)


func _on_friday_toggled(button_pressed):
	if button_pressed:
		days.push_back(5)
	else:
		days = days.filter(func(day): day == 5)


func _on_saturday_toggled(button_pressed):
	if button_pressed:
		days.push_back(6)
	else:
		days = days.filter(func(day): day == 6)


func _on_days_toggled(button_pressed):
	simulationTimescale = "days"
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Minutes.set_pressed_no_signal(false)
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Hours.set_pressed_no_signal(false)


func _on_hours_toggled(button_pressed):
	simulationTimescale = "hours"
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Days.set_pressed_no_signal(false)
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Minutes.set_pressed_no_signal(false)


func _on_minutes_toggled(button_pressed):
	simulationTimescale = "minutes"
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Days.set_pressed_no_signal(false)
	$MarginContainer/VBoxContainer/SimulationTimescale/Buttons/Hours.set_pressed_no_signal(false)
