extends Area3D

var units = -1

var shield = -1

var selected = false

var color

# Called when the node enters the scene tree for the first time.
func _ready():	
	$FloorSprite.texture = $SubViewport.get_texture()
	#$factory.rotation.y = abs(sin(get_parent().getID() * 10.0)) * 2 * PI
	#$factory2.rotation.y = $factory.rotation.y + PI/2 + abs(cos(get_parent().getID() * 20.0)) * PI/4
	
	return
	
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
	if units != get_parent().getUnits(): $SubViewport/OutpostInfo.setUnits(get_parent().getUnits())
	if shield != get_parent().getShield(): $SubViewport/OutpostInfo.setShield(get_parent().getShield(), get_parent().getMaxShield())
	if selected != get_parent().isSelected():
		$RotationInvariant/Units.setSelection(get_parent().isSelected())
		if get_parent().isSelected():
			$FlagSprite.modulate = Color.WHITE
		else:
			color = null
	if color != get_parent().getColor():
		$RotationInvariant/Units.setColor(get_parent().getColor())
		$FlagSprite.modulate = get_parent().getColor()
	
	units = get_parent().getUnits()
	shield = get_parent().getShield()
	selected = get_parent().isSelected()
	color = get_parent().getColor()
	
	if get_parent().isFactory():
		$City/factories.show()
		$City/generators.hide()
	elif get_parent().isGenerator():
		$City/generators.show()
		$City/factories.hide()
