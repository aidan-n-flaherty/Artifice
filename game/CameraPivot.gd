extends Marker3D

signal unselect

var mouse_start_pos
var screen_start_position

var dragging = false

var lastDiff

var momentum = Vector2(0, 0)

var pos = Vector2(0, 0)

var game

var zoom = 0

@export var maxZoom = 1.5

@export var minZoom = 0.5

var selectedNode = null

var target = null

var targetPos = null

func init(gameID: int):
	self.game = GameData.getGame(gameID)
	
	$FloorDisplay.add_child(game.getFloorDisplay())
	
	updatePos()

func updatePos():
	pos = Vector2(position.x, position.z + 100)

	#$Floor.get_surface_override_material(0).set_shader_parameter("offset", pos)
	
	var arr = game.getOutpostPositions()
	$Terrain.material_override.set_shader_parameter("outposts", arr)
	$Terrain.material_override.set_shader_parameter("outpostsLength", len(arr))
	$Terrain.material_override.set_shader_parameter("mapWidth", game.getWidth())
	$Terrain.material_override.set_shader_parameter("mapHeight", game.getHeight())

func selected(node):
	selectedNode = node

func _process(delta):
	if not dragging:
		position.x += momentum.x * 0.9
		position.z += momentum.y * 0.9
		
		while position.x > game.getWidth()/2: position.x -= game.getWidth()
		while position.x <= -game.getWidth()/2: position.x += game.getWidth()
		while position.z > game.getHeight()/2: position.z -= game.getHeight()
		while position.z <= -game.getHeight()/2: position.z += game.getHeight()
		
		if(momentum != Vector2(0, 0)):
			updatePos()
		
		momentum *= 0.9
	
	if game.canStartDrag() and dragging and selectedNode and targetPos:
		target = game.getTarget(targetPos.x, targetPos.y)
		
		var mousePos = targetPos
		if target:
			mousePos = Vector2(target.get_position().x, target.get_position().z)
		
		var time = game.projectedTime(mousePos.x, mousePos.y)
		
		game.setTempTime(time)
		
		game.setDrag(true)
		
		game.setMouse(mousePos.x, mousePos.y)

func _unhandled_input(event):
	if event.is_action("drag"):
		if event.is_pressed():
			mouse_start_pos = event.position
			lastDiff = Vector2(0, 0)
			screen_start_position = Vector2(position.x, position.z)
			dragging = true
		else:
			if selectedNode and event.position == mouse_start_pos and not game.justSelected():
				game.unselect()
			if not selectedNode and event.position == mouse_start_pos and not game.justSelected():
				emit_signal("unselect")
			
			dragging = false
			game.setDrag(false)
			game.setTempTime(0.0)
			if selectedNode and target:
				game.sendTo(target.getID())
				target = null
			
			targetPos = null
	elif event is InputEventMouseMotion and dragging and selectedNode and game.canStartDrag():
		var diff = event.position
		
		diff.x -= get_viewport().get_visible_rect().size.x/2.0
		diff.y -= get_viewport().get_visible_rect().size.y/2.0
		
		diff.x *= $Camera3D.size / 1.0 / get_viewport().get_visible_rect().size.x
		diff.y *= $Camera3D.size / 1.0 / get_viewport().get_visible_rect().size.x * sqrt(2)
		
		targetPos = pos - diff
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
		$FloorDisplay.size = Vector2($Camera3D.size * 10, $Camera3D.size * 26)
		$FloorSprite.scale.z = $Camera3D.size/100.0
		$FloorSprite.scale.x = $Camera3D.size/100.0
		$Terrain.material_override.set_shader_parameter("cameraSize", $Camera3D.size)
		$Terrain.scale.z = $Camera3D.size/100.0
		$Terrain.scale.x = $Camera3D.size/100.0
		$Darkness.scale.z = $Camera3D.size/100.0
		$Darkness.scale.x = $Camera3D.size/100.0
