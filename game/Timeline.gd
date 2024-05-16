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
		
		if userControlled and abs((time_start_pos - change) - Time.get_unix_time_from_system()) < 3600.0 / game.getSimulationSpeed():
			change = time_start_pos - Time.get_unix_time_from_system() + 0.01
			target = time_start_pos - Time.get_unix_time_from_system() + 0.01
			game.setTime(Time.get_unix_time_from_system() + 0.01)
			
			userControlled = dragging
	
	var diff = (Time.get_unix_time_from_system() - game.getTime()) * game.getSimulationSpeed() / 3600.0

	if diff != prevDiff:
		prevDiff = diff
		
		$Horizontal/TimeIndicators/ActualTime.position.x = size.x/2
		$Horizontal/TimeIndicators/CurrentTime.position.x = size.x/2 - $Horizontal/TimeIndicators/CurrentTime.size.x + 8 * diff
		$Vertical/Measurement/Label.position.y = size.y/2
		$Vertical/TimeIndicators/ActualTime.position.y = size.y/2
		$Vertical/TimeIndicators/CurrentTime.position.y = size.y/2 + $Vertical/TimeIndicators/ActualTime.size.x- 8 * diff
		
		for i in len($Horizontal/TimeIndicators/Markers.get_children()):
			var pos = size.x/2 - 2.5 + 8 * diff + i * get_viewport_rect().size.x / 60.0
			
			while pos > get_viewport_rect().size.x:
				pos -= get_viewport_rect().size.x
				
			while pos < 0:
				pos += int(get_viewport_rect().size.x)
			
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).position.x = pos
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).size.y = 20 if i % 4 == 0 else 10
			get_node("Horizontal/TimeIndicators/Markers/Panel" + str(i + 1)).modulate = Color(0.3, 0.3, 0.3) if i % 4 == 0 else Color(0.2, 0.2, 0.2)
		
		for i in len($Vertical/TimeIndicators/Markers.get_children()):
			var pos = size.y/2 - 2.5 - 8 * diff + i * get_viewport_rect().size.y / 60.0
			
			while pos > get_viewport_rect().size.y:
				pos -= get_viewport_rect().size.y
				
			while pos < 0:
				pos += int(get_viewport_rect().size.y)
			
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).position.y = pos
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).size.x = 20 if i % 4 == 0 else 10
			get_node("Vertical/TimeIndicators/Markers/Panel" + str(i + 1)).modulate = Color(0.3, 0.3, 0.3) if i % 4 == 0 else Color(0.2, 0.2, 0.2)

		if abs(Time.get_unix_time_from_system() - game.getTime()) > 0.05:
			$Horizontal/Measurement/VBoxContainer/HBoxContainer/Label.text = Utilities.timeToStr(game.getTime()-Time.get_unix_time_from_system())
			$Vertical/Measurement/Label.text = Utilities.timeToStr(game.getTime()-Time.get_unix_time_from_system())
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
			userControlled = true
		else:
			dragging = false
	elif event is InputEventMouseMotion and dragging:
		if $Horizontal.visible:
			speedScale = 1 / (1 + max(0, -0.5 + 0.1 * abs(event.position.y - mouse_start_pos.y)))
			target = 0.125 * 3600.0 / game.getSimulationSpeed() * (event.position.x - mouse_start_pos.x)
		else:
			speedScale = 1 / (1 + max(0, -0.5 + 0.1 * abs(event.position.x - mouse_start_pos.x)))
			target = 0.125 * 3600.0 / game.getSimulationSpeed() * -(event.position.y - mouse_start_pos.y)
	
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
	moveTo(game.getTime() - 60.0 / game.getSimulationSpeed())


func _on_forward_pressed():
	moveTo(game.getTime() + 60.0 / game.getSimulationSpeed())
