extends MarginContainer

var p1
var playerID
var message

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(p1, playerID:int, message:String, p1Color, p2Color):
	self.message = message
	
	$HBoxContainer2/Control/MarginContainer/RichTextLabel.text = message
	
	
	
	if playerID == p1:
		$HBoxContainer2/RSpacer.show()
		$HBoxContainer2/Control/ColorRect.color = p1Color
	else:
		$HBoxContainer2/LSpacer.show()
		$HBoxContainer2/Control/ColorRect.color = p2Color

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
