extends Control

signal menuSelectionChanged(menuItem)

@export_file("*.tscn") var playScreen
@export_file("*.tscn") var searchScreen
@export_file("*.tscn") var settingsScreen
@export_file("*.tscn") var createScreen

var current = null

var gameID = null

var past = false

var uninitialized = true

# Called when the node enters the scene tree for the first time.
func _ready():
	if not GameData.currentTab:
		GameData.gamesChanged.connect(init)
	else:
		init()

func init():
	if uninitialized:
		uninitialized = false
		
		GameData.current_scene = self
		
		if not GameData.currentTab:
			GameData.currentTab = playScreen
		
		switch_to(GameData.currentTab)
		
		GameData.menuSwitched.connect(switch_to)
		
		GameData.menuFade.connect(menu_fade)
		
		GameData.loadGame.connect(loadGame)
		
		GameData.loadGameDetail.connect(loadGameDetail)
		
		$Background.material.set_shader_parameter("gradStrength", 1.0)
		
		$Fade.modulate = Color(0.0, 0.0, 0.0, 1.0)
		$AnimationPlayer.play("fade_from_black")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	$VSplitContainer/VBoxContainer/UnsafeArea.custom_minimum_size.y = DisplayServer.get_display_safe_area().position.y
	
func switch_to(scene):
	if current:
		$VSplitContainer/VBoxContainer/Content.remove_child(current)
	
	GameData.currentTab = scene
	
	current = ResourceLoader.load(GameData.currentTab).instantiate()
	
	$VSplitContainer/VBoxContainer/Content.add_child(current)

func menu_fade():
	$AnimationPlayer.play("fade_to_black")

func loadGame(gameID: int, past: bool):
	self.gameID = gameID
	self.past = past
	
	$AnimationPlayer.play("fade_to_game")

func loadGameDetail(gameID: int):
	self.gameID = gameID
	
	$AnimationPlayer.play("fade_to_game_detail")

func _on_play_pressed():
	switch_to(playScreen)

func _on_settings_pressed():
	switch_to(settingsScreen)

func _on_search_pressed():
	switch_to(searchScreen)

func _on_create_pressed():
	switch_to(createScreen)

func _on_animation_player_animation_finished(anim_name):
	if anim_name == "fade_to_game":
		GameData.viewGameCompletion(gameID, past)
	elif anim_name == "fade_to_game_detail":
		print("ended")
		GameData.viewGameDetailCompletion(gameID)
