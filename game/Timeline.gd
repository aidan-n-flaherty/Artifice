extends Control

var gameID
var game
var mouse_start_pos
var time_start_pos
var dragging = false

var mouseInComponent = false

var userControlled = false
var change = 0
var target = 0
var speed = 0.1
var cap = -1

var speedScale = 1

var prevDiff = null

# Called when the node enters the scene tree for the first time.
func _ready():
	for i in range(100):
		var node = $Horizontal/TimeIndicators/Markers/Panel1.duplicate()
		node.name = "Panel" + str(i + 2)
		$Horizontal/TimeIndicators/Markers.add_child(node)
		
	for i in range(100):
		var node = $Vertical/TimeIndicators/Markers/Panel1.duplicate()
		node.name = "Panel" + str(i + 2)
		$Vertical/TimeIndicators/Markers.add_child(node)

func init(gameID):
	self.gameID = gameID
	game = GameData.getGame(gameID)
	game.connect("moveTo", moveTo)
	
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Buffer.button_pressed = game.getBuff()
	$Vertical/MarginContainer/VBoxContainer/Buffer.button_pressed = game.getBuff()

func setVertical():
	$Vertical.show()
	$Horizontal.hide()

func setHorizontal():
	$Horizontal.show()
	$Vertical.hide()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if change != target:
		var newChange = (1 - speed) * change + speed * target
		var diff = newChange - change
		if(cap >= 0 && abs(diff) > cap):
			diff *= cap / abs(diff) 
		change += diff
		
		if abs(change - target) < 0.01 * 3600.0 / game.getSimulationSpeed():
			change = target
		game.setTime(time_start_pos - change)
		
		if userControlled and abs((time_start_pos - change) - game.getBuffTime()) < 3600.0 / game.getSimulationSpeed():
			target = time_start_pos - game.getBuffTime() + 0.1
			game.setTime(game.getBuffTime() + 0.1)
			
			userControlled = dragging
			
			if not dragging:
				change = time_start_pos - game.getBuffTime() + 0.1
	
	var diff = (Time.get_unix_time_from_system() - game.getTime()) * game.getSimulationSpeed() / 3600.0

	if diff != prevDiff:
		prevDiff = diff
		
		$Horizontal/TimeIndicators/ActualTime.position.x = size.x/2
		$Horizontal/TimeIndicators/CurrentTime.position.x = size.x/2 - $Horizontal/TimeIndicators/CurrentTime.size.x + 16 * diff
		$Vertical/Measurement/Label.position.y = size.y/2
		$Vertical/TimeIndicators/ActualTime.position.y = size.y/2
		$Vertical/TimeIndicators/CurrentTime.position.y = size.y/2 + $Vertical/TimeIndicators/ActualTime.size.x- 16 * diff
		
		for i in len($Horizontal/TimeIndicators/Markers.get_children()):
			var pos = size.x/2 - 2.5 + 16 * diff + i * get_viewport_rect().size.x / 60.0
			
			if pos > get_viewport_rect().size.x:
				pos -= int(pos / get_viewport_rect().size.x) * get_viewport_rect().size.x
				
			if pos < 0:
				pos += int(ceil(abs(pos / get_viewport_rect().size.x))) * get_viewport_rect().size.x
			
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).position.x = pos
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).size.y = 20 if i % 4 == 0 else 10
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).modulate = Color(0.3, 0.3, 0.3) if i % 4 == 0 else Color(0.2, 0.2, 0.2)
		
		for i in len($Vertical/TimeIndicators/Markers.get_children()):
			var pos = size.y/2 - 2.5 - 16 * diff + i * get_viewport_rect().size.y / 60.0
			
			if pos > get_viewport_rect().size.y:
				pos -= int(pos / get_viewport_rect().size.y) * get_viewport_rect().size.y
				
			if pos < 0:
				pos += int(ceil(abs(pos / get_viewport_rect().size.y))) * get_viewport_rect().size.y
			
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).position.y = pos
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).size.x = 20 if i % 4 == 0 else 10
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).modulate = Color(0.3, 0.3, 0.3) if i % 4 == 0 else Color(0.2, 0.2, 0.2)

		if abs(Time.get_unix_time_from_system() - game.getTime()) > 0.25:
			$Horizontal/Measurement/VBoxContainer/HBoxContainer/Label.text = Utilities.timeToStr(round(game.getTime() - Time.get_unix_time_from_system()))
			$Vertical/Measurement/Label.text = Utilities.timeToStr(round(game.getTime()-Time.get_unix_time_from_system()))
		else:
			$Horizontal/Measurement/VBoxContainer/HBoxContainer/Label.text = ""
			$Vertical/Measurement/Label.text = ""

func _gui_input(event):
	if event is InputEventMouseButton:
		if event.is_pressed():
			mouse_start_pos = event.position
			time_start_pos = game.getTime()
			dragging = true
			change = 0
			target = 0
			speed = 0.1
			cap = -1
			speedScale = 1
		else:
			dragging = false
	elif event is InputEventMouseMotion and dragging:
		userControlled = true
		speed = 1.0
		if $Horizontal.visible:
			speedScale = 1 / (1 + max(0, -0.5 + 0.1 * abs(event.position.y - mouse_start_pos.y)))
			target = 0.0625 * 3600.0 / game.getSimulationSpeed() * (event.position.x - mouse_start_pos.x)
		else:
			speedScale = 1 / (1 + max(0, -0.5 + 0.1 * abs(event.position.x - mouse_start_pos.x)))
			target = 0.0625 * 3600.0 / game.getSimulationSpeed() * -(event.position.y - mouse_start_pos.y)
	
	if dragging:
		accept_event()
		

func _on_mouse_exited():
	mouseInComponent = false

func _on_mouse_entered():
	mouseInComponent = true
	
func moveTo(t):
	userControlled = false
	time_start_pos = game.getTime()
	change = 0
	self.target = time_start_pos - t
	speed = 0.5
	cap = 2.0 * 3600.0 / game.getSimulationSpeed()
#func addMesureLines():
	#Draw lines based off of game.getSimulationSpeed()



func _on_back_pressed():
	moveTo(game.getTime() - max(1.0, 60.0 / game.getSimulationSpeed()))


func _on_forward_pressed():
	moveTo(game.getTime() + max(1.0, 60.0 / game.getSimulationSpeed()))


func _on_buffer_toggled(value: bool):
	game.setBuff(value)
	
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Buffer.text = "Buffer On" if value else "Buffer Off"
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Spacer.text = "Buffer On" if value else "Buffer Off"
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Buffer.add_theme_color_override("font_hover_color", Color.WHITE if value else Color.FIREBRICK)
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Buffer.add_theme_color_override("font_focus_color", Color.WHITE if value else Color.FIREBRICK)
	$Horizontal/Measurement/VBoxContainer/HBoxContainer/Buffer.add_theme_color_override("font_color", Color.WHITE if value else Color.FIREBRICK)
	
	$Vertical/MarginContainer/VBoxContainer/Buffer.text = "On" if value else "Off"
	$Vertical/MarginContainer/VBoxContainer/Spacer.text = "On" if value else "Off"
	$Vertical/MarginContainer/VBoxContainer/Buffer.add_theme_color_override("font_hover_color", Color.WHITE if value else Color.FIREBRICK)
	$Vertical/MarginContainer/VBoxContainer/Buffer.add_theme_color_override("font_focus_color", Color.WHITE if value else Color.FIREBRICK)
	$Vertical/MarginContainer/VBoxContainer/Buffer.add_theme_color_override("font_color", Color.WHITE if value else Color.FIREBRICK)


func _on_revert_to_current_pressed():
	game.setCurrent()
