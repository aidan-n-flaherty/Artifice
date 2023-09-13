extends Area3D

# Called when the node enters the scene tree for the first time.
func _ready():
	"""var heightArr = range(1, 7)
	
	for i in range(6):
		var angle = deg_to_rad(i * 60 + 30)
		addOutpost(angle, 1.7, -1 - 2 * heightArr[i])
		
	for i in range(6):
		var angle = deg_to_rad(i * 60)
		addOutpost(angle, 2 * 1.7, -11)"""
	
	var noise = FastNoiseLite.new()
	
	noise.frequency = 0.15
	noise.seed = get_parent().getID()
	noise.fractal_type = 2
	noise.fractal_octaves = 1
	
	var noise2 = FastNoiseLite.new()
	
	noise2.frequency = 0.15
	noise2.seed = get_parent().getID()
	noise2.noise_type = 0
	
	for x in range(-10, 11):
		for y in range(-10, 11):
			var mag = sqrt(x * x + y * y)
			if(mag <= 3): continue
			var height = min(1 - mag/10.0, -noise2.get_noise_2d(x, y))# + (1 - mag/10.0) * 10.0 * (0.5 + 0.5 * noise.get_noise_2d(x, y))
			if(height <= 0): continue
			if(mag <= 10.0): height += pow(1 - mag/10.0, 3) * 5.0 * (0.5 + 0.5 * noise.get_noise_2d(x, y))
			
			var scalar = 10
			var cube = BoxMesh.new()
			cube.size = Vector3(0.5, scalar * height, 0.5)
			cube.material = preload("res://resources/gradient.tres")
			
			var mesh = MeshInstance3D.new()
			mesh.set_mesh(cube)
			mesh.position = Vector3(x/2.0, scalar * height/2, y/2.0)
			
			$City.add_child(mesh)
	
	$City.rotation.y = deg_to_rad(45)
	
	#for i in range(6):
		#var angle = deg_to_rad(i * 60 + 30)
		#addOutpost(angle, 2 * 1.7, -11)

func addOutpost(angle, dist, height):
	var outpost = load("res://Tower.tscn").instantiate()
	outpost.position = Vector3(dist * cos(angle), height, dist * sin(angle))

	add_child(outpost)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$Label.text = str(get_parent().getUnits())
