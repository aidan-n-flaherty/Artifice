extends MarginContainer


var color = Color.WHITE
var colorP = Color.WHITE
var colorS = Color.WHITE

var bannerID = 0

var playerName = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	get_parent().get_node("Header/HeaderColor/Banner").setPattern(bannerID)
	#get_parent().add_child(banner)
	
	get_parent().get_node("Header/HeaderColor").modulate = color
	get_parent().get_node("Header/MarginContainer/Player").text = playerName
