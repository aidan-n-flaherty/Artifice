extends MarginContainer

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(gameID:int, objectID:int):
	var game
	var players
	var p1
	var p2
	var pUnits
	var phases
	var battleMessages
	
	game = GameData.getGame(gameID)
	
	players = game.getNextBattleUsers(objectID)
	p1 = players[0]
	p2 = players[1]
	
	#set names
	$VBoxContainer/ScrollContainer/VBoxContainer/PlayerDivide/Player1/NameP1.text = p1.getName()
	$VBoxContainer/ScrollContainer/VBoxContainer/PlayerDivide/Player2/NameP2.text = p2.getName()
	
	pUnits = game.getNextBattleStartingUnits(objectID)
	
	#set starting units
	$VBoxContainer/ScrollContainer/VBoxContainer/PlayerDivide/Player1/UnitsP1.text = pUnits[p1.getID()]
	$VBoxContainer/ScrollContainer/VBoxContainer/PlayerDivide/Player2/UnitsP2.text = pUnits[p2.getID()]
	
	phases = game.getBattlePhases()
	
	
	for phase in phases:
		battleMessages = game.getNextBattleMessages(objectID, phase)
		
		var displayPhase
		displayPhase = preload("res://BattleSpecialistEffects.tscn").instantiate()
		displayPhase.init(p1.getID(), phase, battleMessages)
		$VBoxContainer/ScrollContainer/VBoxContainer/Bonuses.add_child(displayPhase)
	
	
		
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
