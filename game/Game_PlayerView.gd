extends Control

var userID = null

var gameID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(id: int, username: String, color: Color, rating: int, gameID: int, ranked: bool, editable: bool):
	self.userID = id
	self.gameID = gameID
	$MarginContainer/HBoxContainer/Name.text = str(username)
	$MarginContainer/HBoxContainer/MarginContainer/Color.modulate = color
	$MarginContainer/HBoxContainer/Name.show()
	$MarginContainer/HBoxContainer/Spacer.hide()
	$MarginContainer/HBoxContainer/MarginContainer/Color.show()
	$MarginContainer/HBoxContainer/Rating.text = str(rating)
	$MarginContainer/HBoxContainer/Rating.visible = ranked
	$MarginContainer/HBoxContainer/Kick.visible = editable and id != GameData.getSelfID()
	
	$View.disabled = id == GameData.getSelfID()
	
	#$ColorPanel.show()
	#color.a = 0.5
	#$ColorPanel.modulate = color
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_view_pressed():
	if userID:
		GameData.viewUser(userID)

func _on_kick_pressed():
	if userID and gameID:
		$MarginContainer/HBoxContainer/Kick.disabled = true
		await GameData.kickUser(gameID, userID)
		$MarginContainer/HBoxContainer/Kick.disabled = false
