extends Marker3D

var mouse_start_pos
var screen_start_position

var dragging = false


func _input(event):
	if event.is_action("drag"):
		if event.is_pressed():
			mouse_start_pos = event.position
			screen_start_position = Vector2(position.x, position.z)
			dragging = true
		else:
			dragging = false
	elif event is InputEventMouseMotion and dragging:
		var newPos = (event.position - mouse_start_pos) + screen_start_position
		position.x = newPos.x
		position.z = newPos.y
