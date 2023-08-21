extends Area3D

@export_file("*.tscn") var outpostMesh

# Called when the node enters the scene tree for the first time.
func _ready():
	var heightArr = range(1, 7)
	
	for i in range(6):
		var angle = deg_to_rad(i * 60 + 30)
		addOutpost(angle, 1.7, -1 - 2 * heightArr[i])
		
	for i in range(6):
		var angle = deg_to_rad(i * 60)
		addOutpost(angle, 2 * 1.7, -11)
		
	#for i in range(6):
		#var angle = deg_to_rad(i * 60 + 30)
		#addOutpost(angle, 2 * 1.7, -11)

func addOutpost(angle, dist, height):
	var outpost = load("res://Tower.tscn").instantiate()
	outpost.position = Vector3(dist * cos(angle), height, dist * sin(angle))

	add_child(outpost)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
