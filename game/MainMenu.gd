extends Control

signal menuSelectionChanged(menuItem)

@export_file("*.tscn") var playScreen
@export_file("*.tscn") var searchScreen
@export_file("*.tscn") var settingsScreen
@export_file("*.tscn") var createScreen

var current

# Called when the node enters the scene tree for the first time.
func _ready():
	if not GameData.currentTab:
		GameData.currentTab = playScreen
	
	current = ResourceLoader.load(GameData.currentTab).instantiate()
	
	$VSplitContainer/Content.add_child(current)
	
	GameData.menuSwitched.connect(switch_to)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass
	
func switch_to(scene):
	$VSplitContainer/Content.remove_child(current)
	
	GameData.currentTab = scene
	
	current = ResourceLoader.load(GameData.currentTab).instantiate()
	
	$VSplitContainer/Content.add_child(current)

func _on_play_pressed():
	switch_to(playScreen)

func _on_settings_pressed():
	switch_to(settingsScreen)

func _on_search_pressed():
	switch_to(searchScreen)

func _on_create_pressed():
	switch_to(createScreen)
