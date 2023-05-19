extends Control

var days = []

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_create_pressed():
	var data = {
		"lobbyName": $MarginContainer/VBoxContainer/LobbyName/Input.text,
		"password": $MarginContainer/VBoxContainer/Password/Input.text,
		"playerCap": $MarginContainer/VBoxContainer/NumPlayers/HSlider.value,
		"simulationSpeed": $MarginContainer/VBoxContainer/SimulationSpeed/HSlider.value,
		"ratingConstraints": [0, $MarginContainer/VBoxContainer/RatingConstraints/HSlider.value],
		"days": days,
		"activeTimes": [str($MarginContainer/VBoxContainer/Time/HSlider.value, "-24")]
	}
	
	GameData.addGame(await HTTPManager.postReq("/createMatch", data))

func _on_sunday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Sunday")
	else:
		days =days.filter(func(day): day == "Sunday")


func _on_monday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Monday")
	else:
		days = days.filter(func(day): day == "Monday")


func _on_tuesday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Tuesday")
	else:
		days = days.filter(func(day): day == "Tuesday")


func _on_wednesday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Wednesday")
	else:
		days = days.filter(func(day): day == "Wednesday")


func _on_thursday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Thursday")
	else:
		days = days.filter(func(day): day == "Thursday")


func _on_friday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Friday")
	else:
		days = days.filter(func(day): day == "Friday")


func _on_saturday_toggled(button_pressed):
	if button_pressed:
		days.push_back("Saturday")
	else:
		days = days.filter(func(day): day == "Saturday")
