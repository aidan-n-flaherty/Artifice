extends Control

var gameID
var game
var mouse_start_pos
var time_start_pos
var dragging = false

var mouseInComponent = false

var change = 0
var target = 0
var speed = 0.1
var cap = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	self.gameID = gameID
	game = GameData.getGame(gameID)
	game.connect("moveTo", moveTo)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if change != target:
		var newChange = (1 - speed) * change + speed * target
		var diff = newChange - change
		if(cap >= 0 && abs(diff) > cap):
			diff *= cap / abs(diff) 
		change += diff
		
		if abs(change - target) < 0.01:
			change = target
		game.setTime(time_start_pos - change)
	
	var diff = Time.get_unix_time_from_system() - game.getTime()
	
	$ActualTime.position = $CurrentTime.position + Vector2(-40 + diff, 0)

func _input(event):
	if event.is_action("drag"):
		if event.is_pressed() and mouseInComponent:
			mouse_start_pos = event.position
			time_start_pos = game.getTime()
			dragging = true
			change = 0
			target = 0
			speed = 0.1
			cap = -1
		else:
			dragging = false
	elif event is InputEventMouseMotion and dragging:
		target = (event.position.x - mouse_start_pos.x)
	
	if dragging:
		accept_event()
		

func _on_mouse_exited():
	mouseInComponent = false

func _on_mouse_entered():
	mouseInComponent = true
	
func moveTo(t):
	time_start_pos = game.getTime()
	change = 0
	self.target = time_start_pos - t
	speed = 0.5
	cap = 1
	


