extends Node3D

var levels = 10

var offset = Vector2()

var cameraSize = 100

@export var chunkSize = 9

@export var pixelsPerMeter = 8

var chunks = Dictionary()


# Called when the node enters the scene tree for the first time.
func _ready():
	pass
	#for i in range(levels):
	#	var mesh = $FloorMesh1.duplicate()
	#	mesh.position.y -= 0.3 * i
	#	mesh.name = "FloorMesh" + str(i + 2)
	#	add_child(mesh)

func generateNoise(x, y):
	if chunks.has(Vector2(x, y)): return
	
	print(str(x) + ", " + str(y))
	
	var newTile = preload("FloorTile.tscn").instantiate()
	newTile.position = Vector3(chunkSize * x, 0, chunkSize * y)
	
	var landNoise = FastNoiseLite.new()
	
	landNoise.frequency = 0.005
	landNoise.seed = 0
	landNoise.fractal_type = 2
	landNoise.domain_warp_enabled = true
	
	var oceanNoise = FastNoiseLite.new()

	oceanNoise.seed = 0
	oceanNoise.frequency = 0.005
	oceanNoise.domain_warp_enabled = true
		
	landNoise.offset = Vector3(pixelsPerMeter * chunkSize * x, pixelsPerMeter * chunkSize * y, 0)
	oceanNoise.offset = Vector3(pixelsPerMeter * chunkSize * x, pixelsPerMeter * chunkSize * y, 0)
	
	var landImg = NoiseTexture2D.new()
	landImg.normalize = false
	landImg.width = pixelsPerMeter * (chunkSize + 1)
	landImg.height = 8 * (chunkSize + 1)
	landImg.noise = landNoise
	
	await landImg.changed
	
	var oceanImg = NoiseTexture2D.new()
	oceanImg.normalize = false
	oceanImg.width = pixelsPerMeter * (chunkSize + 1)
	oceanImg.height = pixelsPerMeter * (chunkSize + 1)
	oceanImg.noise = oceanNoise
	
	await oceanImg.changed
	
	for i in range(levels - 1):
		var mesh = newTile.get_node("FloorMesh1").duplicate()
		mesh.position.y -= 0.75 * i
		mesh.name = "FloorMesh" + str(i + 2)
		newTile.add_child(mesh)
		
	for i in range(levels):
		newTile.get_node("FloorMesh" + str(i + 1)).get_surface_override_material(0).set_shader_parameter("land", landImg)
		newTile.get_node("FloorMesh" + str(i + 1)).get_surface_override_material(0).set_shader_parameter("ocean", oceanImg)
		
	newTile.get_node("FloorMeshSurface").get_surface_override_material(0).set_shader_parameter("land", landImg)
	newTile.get_node("FloorMeshSurface").get_surface_override_material(0).set_shader_parameter("ocean", oceanImg)
	
	add_child(newTile)
	if(chunks.has(Vector2(x, y))): remove_child(chunks[Vector2(x, y)])
	chunks[Vector2(x, y)] = newTile

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	for x in range(floor((-offset.x - cameraSize/2)/chunkSize) - 1, ceil((-offset.x + cameraSize/2)/chunkSize) + 1):
		for y in range(floor((-offset.y - 2.6/2 * cameraSize)/chunkSize) - 1, ceil((-offset.y + 2.6/2 * cameraSize)/chunkSize) + 1):
			call_deferred("generateNoise", x, y)
