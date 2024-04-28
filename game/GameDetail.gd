extends GameEditor

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID):
	setEditable(false)
	
	$Background.material.set_shader_parameter("gradStrength", 0.75)
	
	$Fade.modulate = Color(1.0, 1.0, 1.0)
	
	await super.deserialize(gameID)
	
	$AnimationPlayer.play("fade_from_black")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_back_button_pressed():
	$AnimationPlayer.play("fade_to_black")

func _on_join_pressed():
	if await GameData.joinGame(gameID, $MarginContainer/VBoxContainer/Passworded/PasswordText.text):
		$AnimationPlayer.play("fade_to_game")


func _on_animation_player_animation_finished(anim_name):
	if anim_name == "fade_to_game":
		GameData.viewGameCompletion(gameID, false)
	if anim_name == "fade_to_black":
		GameData.goto_scene("res://MainMenu.tscn")
