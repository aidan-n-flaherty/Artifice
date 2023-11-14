extends MarginContainer

var shield = 0

var maxShield = 0

var easeShield = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if easeShield != shield:
		easeShield = 0.5 * easeShield + 0.5 * shield
		
		if abs(easeShield - shield) < 0.001:
			easeShield = shield
		
		queue_redraw()
	
func setShield(shield: int, maxShield: int):
	self.shield = shield
	self.maxShield = maxShield
	$Shield.text = str(shield)

func _draw():
	var center = Vector2(size.x/2, size.y/2)
	
	if maxShield > 40:
		var fill = easeShield*1.0/maxShield

		var radius = size.x * 2/7
		var angle_from = 45
		var angle_to = angle_from + 160
		var color = Color(0.0, 0.0, 0.0, 0.2)
		var width = 25
		draw_arc(center, radius, deg_to_rad(angle_from), deg_to_rad(angle_to), 16, color, width)
		
		color = Color(0.0, 0.0, 0.0, 1.0)
		angle_to = angle_from + 160 * fill
		draw_arc(center, radius, deg_to_rad(angle_from), deg_to_rad(angle_to), 16, color, width)
		
		return
	
	for i in range(floor(maxShield/10)):
		var fill = min(10, easeShield - i * 10)/10.0
		
		var radius = size.x/4 + i * 10
		var angle_from = 45
		var angle_to = angle_from + 160
		var color = Color(0.0, 0.0, 0.0, 0.2)
		var width = 5
		draw_arc(center, radius, deg_to_rad(angle_from), deg_to_rad(angle_to), 16, color, width)

		if fill <= 0:
			continue
			
		color = Color(0.0, 0.0, 0.0, 1.0)
		angle_to = angle_from + 160 * fill
		draw_arc(center, radius, deg_to_rad(angle_from), deg_to_rad(angle_to), 16, color, width)
