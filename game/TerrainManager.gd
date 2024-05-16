extends Node3D

var game

var tileSize = 5

func _ready():
	pass

func init(gameID: int):
	self.game = GameData.getGame(gameID)
	
	# generateFloor(self.game.getWidth(), self.game.getHeight())
	
	# generateDetails(self.game.getWidth(), self.game.getHeight())
	
	for x in range(-1, 2):
		for y in range(-1, 2):
			if x == 0 and y == 0:
				continue
				
			var node = $TerrainBlock.duplicate()
			node.position.x = x * self.game.getWidth()
			node.position.z = y * self.game.getHeight()
			add_child(node)


func generateFloor(width: int, height: int):
	var noise = preload("res://resources/noise/TerrainNoise2.tres")
	
	var multimesh = MultiMesh.new()
	multimesh.transform_format = MultiMesh.TRANSFORM_3D
	multimesh.instance_count = (1 + ceil(width / sqrt(2) / tileSize)) * (1 + ceil(height * 2 / sqrt(2) / tileSize))
	multimesh.visible_instance_count = multimesh.instance_count
	
	multimesh.mesh = $Elements/FloorCube.mesh
	
	var image = noise.get_image()

	var i = 0
	# Set the transform of the instances.
	for x in range(0, 1 + ceil(width / sqrt(2) / tileSize)):
		for y in range(0, 1 + ceil(height * 2 / sqrt(2) / tileSize)):
			var x1 = x * tileSize * sqrt(2)
			var y1 = y * tileSize * sqrt(2) / 2
			if y % 2 == 0:
				x1 += tileSize * sqrt(2) /2
			
			var h = int(image.get_pixel(int(x1) % image.get_width(), int(y1) % image.get_height()).r * 4.0) / 4.0
			
			multimesh.set_instance_transform(i, Transform3D(Basis(), Vector3(0.0, 0.0, 0.0)).rotated(Vector3(0.0, 1.0, 0.0), PI/4).scaled(Vector3(tileSize, 1.0, tileSize)).translated(Vector3(x1, -h * 4.0, y1)))
			i += 1
		
	$TerrainBlock/Floor.multimesh = multimesh

func generateDetails(width: int, height: int):
	var noise = preload("res://resources/noise/TerrainNoise.tres")
	#var noise2 = preload("res://resources/noise/TerrainNoise2.tres")
	
	var multimesh = MultiMesh.new()
	multimesh.transform_format = MultiMesh.TRANSFORM_3D
	multimesh.instance_count = width * height / 8
	multimesh.visible_instance_count = multimesh.instance_count
	
	multimesh.mesh = $Elements/BeveledCube.mesh

	var random = RandomNumberGenerator.new()
	random.randomize()
	
	var image = noise.get_image()
	#var image2 = noise2.get_image()

	for i in multimesh.visible_instance_count:
		var x = random.randi_range(0, width)
		var y = random.randi_range(0, height)
		
		while random.randf() > image.get_pixel(x % image.get_width(), y % image.get_height()).r:
			x = random.randi_range(0, width)
			y = random.randi_range(0, height)
		
		var scaleH = random.randf_range(1.0, 4.0)
		#var h = image2.get_pixel(int(x) % image.get_width(), int(y) % image.get_height()).r
		
		multimesh.set_instance_transform(i, Transform3D(Basis(), Vector3(0.0, 0.0, 0.0)).rotated(Vector3(0.0, 1.0, 0.0), 0.0 * 2 * PI * random.randf()).scaled(Vector3(scaleH, random.randf_range(0.5, 1.0 + pow(4.0/scaleH, 2)), scaleH)).translated(Vector3(x, 0, y)))
		
	$TerrainBlock/Details.multimesh = multimesh


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
