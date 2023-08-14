extends Control

var time = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	time += delta
	
	queue_redraw()

func _draw():
	var center = Vector2(512, 512)
	var radius = 160
	
	var angle_from = 30 * time
	while(angle_from > 360):
		angle_from -= 360
		
	draw_circle_arc(center, radius, angle_from * 2, angle_from * 2 + 120, Color(1, 1, 1))
	draw_circle_arc(center, radius + 30, angle_from, angle_from + 300, Color(1, 1, 1))
	
	
	#for i in range(6):
	#	draw_triangle(center, radius + 20 * cos(time), deg_to_rad(i * 60 + 20 * time), Color(1, 1, 1))
	
func draw_triangle(center, distance, angle, color):
	var points = PackedVector2Array()
	var centerPoint = center + Vector2(cos(angle), sin(angle)) * distance
	
	for i in range(3):
		var angle_point = angle + deg_to_rad(i * 120)
		points.push_back(centerPoint + 40 * Vector2(cos(angle_point), sin(angle_point)))
	
	var colors = PackedColorArray([color])

	draw_polygon(points, colors)
	
func draw_circle_arc(center, radius, angle_from, angle_to, color):
	var nb_points = 32
	var points_arc = PackedVector2Array()

	for i in range(nb_points + 1):
		var angle_point = deg_to_rad(angle_from + i * (angle_to-angle_from) / nb_points - 90)
		points_arc.push_back(center + Vector2(cos(angle_point), sin(angle_point)) * radius)

	for index_point in range(nb_points):
		draw_line(points_arc[index_point], points_arc[index_point + 1], color, 20)
