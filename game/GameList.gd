extends MarginContainer
class_name GameList

@export_file("*.tscn") var GameButton

var gameIDs
	
# Called when the node enters the scene tree for the first time.
func _ready():
	var node = get_node("ScrollContainer/Content")
	
	for id in gameIDs:
		node.add_child(generateButton(id))

func generateButton(id) -> Button:
	var button = GameButton.new()
	button.text = str("Game: ", str(id))
	
	return button

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
