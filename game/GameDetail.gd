extends GameEditor

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID):
	setEditable(false)
	
	super.deserialize(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_back_button_pressed():
	GameData.goto_scene("res://MainMenu.tscn")

func _on_join_pressed():
	if await GameData.joinGame(gameID, $MarginContainer/VBoxContainer/Passworded/PasswordText.text):
		GameData.goto_scene("res://MainMenu.tscn")
