extends MarginContainer

var userID: int

var color: Color

var selected: bool

var modifiable: bool

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(player, selected: bool, modifiable: bool):
	self.selected = selected
	userID = player.getUserID()
	color = player.getColor()
	
	$Button.button_pressed = selected
	$Button.disabled = not modifiable
	
	$MarginContainer/Label.text = player.getName()
	
	$Button.modulate = color if selected else Color(0.0, 0.0, 0.0)
	$Panel.modulate = color

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func isSelected() -> bool:
	return selected

func getUserID() -> int:
	return userID

func _on_button_toggled(button_pressed):
	selected = button_pressed
	
	$Button.modulate = color if selected else Color(0.0, 0.0, 0.0)
