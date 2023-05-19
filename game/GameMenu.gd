extends MarginContainer

@export_file("*.tscn") var MenuScreen
@export_file("*.tscn") var OpenGameList

func _init():
	GameData.loadGames()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_back_pressed():
	GameData.goto_scene(MenuScreen)

func setItem(item):
	var node = get_node("VBoxOuter/VBoxInner/Content")
	
	for child in node.get_children():
		child.queue_free()
	
	node.add_child(item)

func _on_progress_pressed():
	setItem(load(OpenGameList).instantiate())

func _on_open_pressed():
	setItem(load(OpenGameList).instantiate())
