extends MarginContainer

var gameID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(userID: int):
	$Fade.modulate = Color(1.0, 1.0, 1.0, 1.0)
	
	GameData.loadGame.connect(loadGame)
	
	await $VBoxContainer/AccountDetails.init(userID)
	
	$AnimationPlayer.play("fade_from_black")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_back_button_pressed():
	$AnimationPlayer.play("fade_to_black")

func loadGame(gameID: int, past: bool):
	self.gameID = gameID
	
	$AnimationPlayer.play("fade_to_game")
	
func _on_animation_player_animation_finished(anim_name):
	if anim_name == "fade_to_game":
		GameData.viewGameCompletion(gameID, true)
	if anim_name == "fade_to_black":
		if not GameData.hasGame(GameData.getCurrentGameID()):
			GameData.viewGameDetailCompletion(GameData.getCurrentGameID())
		else:
			GameData.viewGameCompletion(GameData.getCurrentGameID(), false)			
