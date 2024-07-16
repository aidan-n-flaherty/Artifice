extends Control

var userID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(id: int, username: String, rating: int, place: int):
	self.userID = id
	$HBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Name.text = str(username)
	$HBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Name.show()
	$HBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Rating.text = str(rating)
	$HBoxContainer/NumContainer/MarginContainer/Number.text = str(place)
	
	$HBoxContainer/MarginContainer/View.disabled = id == GameData.getSelfID()
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_view_pressed():
	if userID:
		GameData.viewUser(userID)
