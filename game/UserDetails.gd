extends MarginContainer

var gameID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(userID: int):
	await $VBoxContainer/AccountDetails.init(userID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_back_button_pressed():
	GameData.previous()
