extends MarginContainer

var userID: int

var gameID: int

var past: bool

var currentScene = null

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.rootScene = self
	
	$Fade.modulate = Color(1.0, 1.0, 1.0, 1.0)
	
	GameData.loadUserDetail.connect(viewUser)
	GameData.loadGameDetail.connect(viewGameDetail)
	GameData.loadGame.connect(viewGame)
	GameData.loadPrevious.connect(viewPrevious)
	GameData.loadCurrent.connect(viewCurrent)

func viewGame(gameID: int, past: bool):
	self.gameID = gameID
	self.past = past
	
	$AnimationPlayer.play("fade_to_game")
	
	print("Playing...")

func viewUser(userID: int):
	self.userID = userID
	
	$AnimationPlayer.play("fade_to_user")

func viewGameDetail(gameID: int):
	self.gameID = gameID
	
	$AnimationPlayer.play("fade_to_game_detail")

func viewPrevious():
	$AnimationPlayer.play("fade_to_previous")

func viewCurrent():
	$AnimationPlayer.play("fade_to_current")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setScene(scene):
	if currentScene:
		currentScene.queue_free()
		$MarginContainer.remove_child(currentScene)
	
	$MarginContainer.add_child(scene)
	currentScene = scene
	
	$AnimationPlayer.play("fade_from_black")

func _on_animation_player_animation_finished(anim_name):
	if anim_name == "fade_to_user":
		GameData.viewUserCompletion(userID)
	elif anim_name == "fade_to_game":
		GameData.viewGameCompletion(gameID, past)
	elif anim_name == "fade_to_game_detail":
		GameData.viewGameDetailCompletion(gameID)
	elif anim_name == "fade_to_previous":
		GameData.goto_previous()
	elif anim_name == "fade_to_current":
		GameData.currentTab = "res://CurrentGameList.tscn"
		GameData.goto_scene("res://MainMenu.tscn")
