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

var zoomStart = 0

@export var maxZoom = 1.8

@export var minZoom = 0.6

var selectedNode = null

var target = null

var targetPos = null

var cameraSize

var touchEventStart = []

var touchEventCurrent = []

func _ready():
	$FloorSprite.material_override.set_shader_parameter("screen_texture", $FloorDisplay.get_texture())
	get_viewport().connect("size_changed", resize)
	
	resize()

func resize():
	var cameraX = 100.0 * (pow(get_viewport().size.x, 0.5) * 1.5 / pow(get_viewport().size.y, 0.5)) * ((minZoom + maxZoom)/2.0 + tanh(zoom) * (maxZoom - minZoom) / 2.0)
	var cameraY = 100.0 * (pow(get_viewport().size.y, 0.5) * 3.0 / pow(get_viewport().size.x, 0.5))  * ((minZoom + maxZoom)/2.0 + tanh(zoom) * (maxZoom - minZoom) / 2.0)
	
	$Camera3D.size = cameraX
	$FloorDisplay.size = Vector2(10 * cameraX, 10 * cameraY)
	$FloorSprite.scale.z = cameraY/100.0
	$FloorSprite.scale.x = cameraX/100.0
	$Terrain.mesh.material.set_shader_parameter("cameraSize", cameraX)
	$Terrain.mesh.material.set_shader_parameter("meshHeight", cameraY)
	$Terrain.mesh.material.set_shader_parameter("meshWidth", cameraX)
	$Terrain.scale.z = cameraY/100.0
	$Terrain.scale.x = cameraX/100.0
	
	#get_parent().get_node("WorldEnvironment").camera_attributes.dof_blur_far_distance = 315 * max(1.0, sqrt($Camera3D.size/100.0))

func init(gameID: int):
	self.game = GameData.getGame(gameID)
	$Terrain.mesh.material.set_shader_parameter("mapWidth", game.getWidth())
	$Terrain.mesh.material.set_shader_parameter("mapHeight", game.getHeight())
	
	if GameData.localSettings.has("graphics") and GameData.localSettings["graphics"] == "simple":
		$Terrain.mesh.material.set_shader_parameter("simple", true)
	else:
		$Terrain.mesh.material.set_shader_parameter("simple", false)
	
	$FloorDisplay.add_child(game.getFloorDisplay())
	
	var startPos = self.game.getSpawnLocation()
	
	position.x = startPos.x
	position.z = startPos.y - 100
	
	updatePos()

func updatePos():
	pos = Vector2(position.x, position.z + 100)

	#$Floor.get_surface_override_material(0).set_shader_parameter("offset", pos)
	
	#var arr = game.getOutpostPositions()
	#$Terrain.material_override.set_shader_parameter("outposts", arr)
	#$Terrain.material_override.set_shader_parameter("outpostsLength", len(arr))

func selected(node):
	selectedNode = node

func normalize(vec):
	while vec.x >= game.getWidth(): vec.x -= game.getWidth()
	while vec.x < 0: vec.x += game.getWidth()
	while vec.y >= game.getHeight()/2: vec.y -= game.getHeight()
	while vec.y < -game.getHeight()/2: vec.y += game.getHeight()
	
	return vec

func _process(delta):
	if not dragging:
		position.x += momentum.x * 0.9
		position.z += momentum.y * 0.9
		
		position.x = normalize(Vector2(position.x, position.z)).x
		position.z = normalize(Vector2(position.x, position.z)).y
		
		if(momentum != Vector2(0, 0)):
			updatePos()
		
		momentum *= 0.9
	
	if game.canStartDrag() and dragging and selectedNode and targetPos:
		if abs(lastDiff.x) > 0.75:
			position.x -= sign(lastDiff.x) * pow(8.0 * (lastDiff.x - sign(lastDiff.x) * 0.75), 2)
			targetPos.x -= sign(lastDiff.x) * pow(8.0 * (lastDiff.x - sign(lastDiff.x) * 0.75), 2)
		if abs(lastDiff.y) > 0.75:
			position.z -= sign(lastDiff.y) * pow(8.0 * (lastDiff.y - sign(lastDiff.y) * 0.75), 2)
			targetPos.y -= sign(lastDiff.y) * pow(8.0 * (lastDiff.y - sign(lastDiff.y) * 0.75), 2)
		
		position.x = normalize(Vector2(position.x, position.z)).x
		position.z = normalize(Vector2(position.x, position.z)).y
		
		targetPos = normalize(targetPos)
		
		updatePos()
		
		target = game.getTarget(targetPos.x, targetPos.y)
		
		var mousePos = targetPos
		if target:
			mousePos = Vector2(target.get_position().x, target.get_position().z)
		
		var time = game.projectedTime(mousePos.x, mousePos.y)
		
		game.setTempTime(time)
		
		game.setDrag(true)
		
		game.setMouse(mousePos.x, mousePos.y)

func _unhandled_input(event):
	if event is InputEventMouseButton:
		if event.is_pressed():
			mouse_start_pos = event.position
			lastDiff = Vector2(0, 0)
			screen_start_position = Vector2(position.x, position.z)
			dragging = true
		elif dragging:
			if selectedNode and (not mouse_start_pos or event.position.distance_to(mouse_start_pos) < 1) and not game.justSelected():
				game.unselect()
				emit_signal("unselect")
			if not selectedNode and (not mouse_start_pos or event.position.distance_to(mouse_start_pos) < 1) and not game.justSelected():
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
		
		lastDiff = diff / (get_viewport().get_visible_rect().size / 2.0)
		
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
		
		var newPos = normalize(diff + screen_start_position)
		
		position.x = newPos.x
		position.z = newPos.y
		
		updatePos()
	elif event is InputEventPanGesture:
		var diff = event.delta.y
		
		zoom += 0.1 * diff
		if(zoom > 3.0): zoom = 3.0
		elif(zoom < -3.0): zoom = -3.0
		
		resize()
	
	if event is InputEventScreenTouch:
		if not event.pressed:
			touchEventStart.remove_at(event.index)
			touchEventCurrent.remove_at(event.index)
		elif event.index >= len(touchEventStart):
			touchEventStart.append(event.position)
			touchEventCurrent.append(event.position)
			
			if event.index > 0:
				dragging = false
				zoomStart = zoom
	if event is InputEventScreenDrag and len(touchEventStart) == 1:
		touchEventStart[event.index] = event.position
	if event is InputEventScreenDrag and len(touchEventStart) == 2:
		var dist1 = touchEventStart[0].distance_to(touchEventStart[1])
		
		touchEventCurrent[event.index] = event.position
		
		var dist2 = touchEventCurrent[0].distance_to(touchEventCurrent[1])
		
		zoom = zoomStart + 3.0 * log(dist1/dist2)/log(2)
		if(zoom > 3.0): zoom = 3.0
		elif(zoom < -3.0): zoom = -3.0
		
		resize()
		

