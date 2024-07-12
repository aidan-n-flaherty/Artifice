extends GameEditor

var onlineMenu = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	setEditable(true)
	playersVisible(false)
	
	super._ready()

func activate():
	if onlineMenu == 0:
		var game = await createGame()
		
		if game:
			GameData.currentTab = "res://CurrentGameList.tscn"
			GameData.goto_scene("res://MainMenu.tscn")
	
	if onlineMenu == 1:
		GameData.viewGame(-2, false)
	
	if onlineMenu == 2:
		GameData.viewGame(-1, false)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_online_pressed() -> void:
	if onlineMenu == 1:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyName.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Ranked.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescale.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Rating.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.show()
	if onlineMenu == 2:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyName.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Ranked.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayers.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameMode.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescale.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Rating.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamCount.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.show()
	
	onlineMenu = 0

func _on_singleplayer_pressed() -> void:
	if onlineMenu == 0:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyName.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Ranked.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescale.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Rating.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.hide()
	
	if onlineMenu == 2:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayers.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameMode.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamCount.show()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.show()
	
	onlineMenu = 1


func _on_sandbox_pressed() -> void:
	if onlineMenu == 0:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyName.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/LobbyNameText.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Password.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/PasswordText.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Ranked.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RankedButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayers.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameMode.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescale.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/SimulationTimescaleButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHours.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/ActiveHoursRows.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/Rating.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/RatingSlider.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamCount.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.hide()
	
	if onlineMenu == 1:
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayers.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/NumPlayersButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameMode.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/GameModeButtons.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamCount.hide()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons.hide()
	
	onlineMenu = 2
