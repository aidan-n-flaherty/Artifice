extends MarginContainer

signal cancel(orderID: int)

signal shiftToTime(timestamp: float)

var id: int

var timestamp: float

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(id: int, timestamp: float, description: String, invalid: bool, canUndo: bool):
	self.id = id
	self.timestamp = timestamp
	$MarginContainer/HBoxContainer/VBoxContainer/Time.text = Utilities.timeToDateStr(timestamp)
	$MarginContainer/HBoxContainer/VBoxContainer/Description.text = description
	$Panel.modulate = Color.RED if invalid else Color.BLACK
	$MarginContainer/HBoxContainer/Cancel.disabled = not canUndo

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_cancel_pressed():
	emit_signal("cancel", id)


func _on_jump_pressed():
	if timestamp >= 0:
		emit_signal("shiftToTime", timestamp)
