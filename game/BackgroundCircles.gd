extends Control


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	queue_redraw()

func _draw():
	var count = int($"../..".get_parent().getShield())
	
	var center = Vector2(512, 512)
	var radius = 256
	var angle_from = 135
	var angle_size = 180
	
	for i in 1 + count/10:
		draw_circle_arc(center, radius + i * 30, angle_from, angle_from + angle_size, Color(0, 0, 0, 0.2))
		draw_circle_arc(center, radius + i * 30, angle_from, angle_from + min(10, count - i * 10) * angle_size / 10, Color(0, 0, 0, 1))
	
func draw_circle_arc(center, radius, angle_from, angle_to, color):
	var nb_points = 32
	var points_arc = PackedVector2Array()

	for i in range(nb_points + 1):
		var angle_point = deg_to_rad(angle_from + i * (angle_to-angle_from) / nb_points - 90)
		points_arc.push_back(center + Vector2(cos(angle_point), sin(angle_point)) * radius)

	for index_point in range(nb_points):
		draw_line(points_arc[index_point], points_arc[index_point + 1], color, 20)
