extends Control

signal menuSelectionChanged(menuItem)

@export_file("*.tscn") var playScreen
@export_file("*.tscn") var searchScreen
@export_file("*.tscn") var historyScreen
@export_file("*.tscn") var settingsScreen
@export_file("*.tscn") var createScreen

var current

# Called when the node enters the scene tree for the first time.
func _ready():
	current = ResourceLoader.load(playScreen).instantiate()
	
	get_node("Content").add_child(current)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass
	
func switch_to(scene):
	get_node("Content").remove_child(current)
	
	current = ResourceLoader.load(scene).instantiate()
	
	get_node("Content").add_child(current)

func _on_play_pressed():
	switch_to(playScreen)

func _on_history_pressed():
	switch_to(historyScreen)

func _on_settings_pressed():
	switch_to(settingsScreen)

func _on_search_pressed():
	switch_to(searchScreen)

func _on_create_pressed():
	switch_to(createScreen)
