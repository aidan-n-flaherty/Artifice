extends Area3D

# Called when the node enters the scene tree for the first time.
func _ready():	
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
			#cube.material = preload("res://resources/metal.tres")
			var mesh = MeshInstance3D.new()
			mesh.set_mesh(cube)
			mesh.position = Vector3(x/2.0, scalar * height/2, y/2.0)
			
			$City.add_child(mesh)
	
	$City.rotation.y = deg_to_rad(45)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$RotationInvariant/Units.setValue(str(get_parent().getUnits()))
	$RotationInvariant/Shield.setValue(str(get_parent().getShield()))
	$RotationInvariant/Units.setSelection(get_parent().isSelected())
	$RotationInvariant/Units.setColor(get_parent().getColor())
