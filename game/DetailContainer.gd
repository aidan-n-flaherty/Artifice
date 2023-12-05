extends MarginContainer

var color = Color.WHITE

var playerName = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	get_parent().get_node("Header/HeaderColor").modulate = color
	get_parent().get_node("Header/MarginContainer/Player").text = playerName
