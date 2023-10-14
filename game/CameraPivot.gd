extends Marker3D

var mouse_start_pos
var screen_start_position

var dragging = false

var lastDiff

var momentum = Vector2(0, 0)

var pos

var game

var zoom = 0

@export var maxZoom = 1.5

@export var minZoom = 0.5

func updatePos():
	pos = Vector2(position.x, position.z + 100)

	#$Floor.get_surface_override_material(0).set_shader_parameter("offset", pos)
	
	#var arr = game.getOutpostPositions();
	#$Floor.get_surface_override_material(0).set_shader_parameter("outposts", arr)
	#$Floor.get_surface_override_material(0).set_shader_parameter("outpostsLength", arr.size())

func _process(delta):
	if not dragging:
		position.x += momentum.x * 0.9
		position.z += momentum.y * 0.9
		
		if(momentum != Vector2(0, 0)):
			updatePos()
		
		momentum *= 0.9
	
	pass

func _unhandled_input(event):
	if event.is_action("drag"):
		if event.is_pressed():
			mouse_start_pos = event.position
			lastDiff = Vector2(0, 0)
			screen_start_position = Vector2(position.x, position.z)
			dragging = true
		else:
			dragging = false
	elif event is InputEventMouseMotion and dragging:
		var diff = event.position - mouse_start_pos
		#print(diff * $Camera3D.size / 1.0 / get_viewport().get_visible_rect().size)
		diff.x *= $Camera3D.size / 1.0 / get_viewport().get_visible_rect().size.x
		diff.y *= $Camera3D.size / 1.0 / get_viewport().get_visible_rect().size.x * sqrt(2)
		
		momentum = diff - lastDiff
		lastDiff = diff
		
		var newPos = diff + screen_start_position
		
		while newPos.x > game.getWidth()/2: newPos.x -= game.getWidth()
		while newPos.x <= -game.getWidth()/2: newPos.x += game.getWidth()
		while newPos.y > game.getHeight()/2: newPos.y -= game.getHeight()
		while newPos.y <= -game.getHeight()/2: newPos.y += game.getHeight()
		
		position.x = newPos.x
		position.z = newPos.y
		
		updatePos()
	elif event is InputEventPanGesture:
		var diff = event.delta.y
		
		zoom += 0.1 * diff
		if(zoom > 3.0): zoom = 3.0
		elif(zoom < -3.0): zoom = -3.0
		
		$Camera3D.size = 100 * ((minZoom + maxZoom)/2.0 + tanh(zoom) * (maxZoom - minZoom) / PI)
		$SubViewport/VirtualCamera3D.size = $Camera3D.size
		$Reflection.scale.z = -$Camera3D.size/100.0
		$Reflection.scale.x = -$Camera3D.size/100.0
		$Reflection.get_surface_override_material(0).set_shader_parameter("cameraSize", $Camera3D.size)
		$Floor/Floor1.get_surface_override_material(0).set_shader_parameter("cameraSize", $Camera3D.size)
		$Floor.scale.z = $Camera3D.size/100.0
		$Floor.scale.x = $Camera3D.size/100.0
		$Terrain.get_surface_override_material(0).set_shader_parameter("cameraSize", $Camera3D.size)
		$Terrain.scale.z = $Camera3D.size/100.0
		$Terrain.scale.x = $Camera3D.size/100.0
