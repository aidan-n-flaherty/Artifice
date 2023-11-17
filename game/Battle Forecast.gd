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
	if (len(players) == 0):
		print("None here!")
		return

	p1 = players[0]
	p2 = players[1]
	
	$VBoxContainer/ScrollContainer/HBoxContainer/p1Color.color = p1.getColor()
	$VBoxContainer/ScrollContainer/HBoxContainer/p2Color.color = p2.getColor()
	
	#set names
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/PlayerDivide/Player1/NameP1.text = p1.getName()
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/PlayerDivide/Player2/NameP2.text = p2.getName()
	
	pUnits = game.getNextBattleStartingUnits(objectID)
	var p1Units = pUnits[p1.getID()]
	var p2Units = pUnits[p2.getID()]
	
	#set starting units
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/PlayerDivide/Player1/HBoxContainer/UnitsP1.text = str(p1Units)
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/PlayerDivide/Player2/HBoxContainer/UnitsP2.text = str(p2Units)
	
	phases = game.getBattlePhases()

	var postPhases = []
	
	for phase in phases:
		if (phase == "Combat Resolution Phase" || phase == "Post-Combat Phase"):
			postPhases.push_back(phase)
			continue
		
		battleMessages = game.getNextBattleMessages(objectID, phase)
		print(battleMessages)

		if (len(battleMessages) != 0):
			var displayPhase
			displayPhase = preload("res://BattleSpecialistEffects.tscn").instantiate()
			displayPhase.init(p1.getID(), phase, battleMessages, p1.getColor(), p2.getColor())
			$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Bonuses.add_child(displayPhase)
	
	#whatever to power/final
	
	#power
	print("Check!")
	#get unit counts
	
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player1/VBoxContainer/Units/UnitsP1.text = str(p1Units)
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player2/VBoxContainer/Units/UnitsP2.text = str(p2Units)
	
	#get shield charge (getters not in C++) 
		#game.getNextBattleShields(objectID)
	
	var shieldCharges = game.getNextBattleShields(objectID)
	var p1ShieldCharge = shieldCharges[p1.getID()]
	var p2ShieldCharge = shieldCharges[p2.getID()]
	
	if p1ShieldCharge == 0:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player1/VBoxContainer/Charge.hide()
	else:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player1/VBoxContainer/Charge/ChargeP1.text = str(p1ShieldCharge)
		
	if p2ShieldCharge == 0:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player2/VBoxContainer/Charge.hide()
	else:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player2/VBoxContainer/Charge/ChargeP2.text = str(p2ShieldCharge)
	
	#get total power (calculated gdscript side)
	
	var p1Power = p1Units + p1ShieldCharge
	var p2Power = p2Units + p2ShieldCharge
	
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player1/VBoxContainer/Power/PowerP1.text = str(p1Power)
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/Power/Player2/VBoxContainer/Power/PowerP2.text = str(p2Power)
	
	#final result
	#get winner (no bind) 
		#game.getNextBattleVictor(objectID)
	var victor = game.getNextBattleVictor(objectID)
	
	var loser
	var victorPower
	var loserPower
	if (victor == null) or (victor.getID() == p1.getID()):
		loser = p2
		victorPower = p1Power
		loserPower = p2Power
	else:
		loser = p1
		victorPower = p2Power
		loserPower = p1Power
	

	
	if victor:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/WinnerStatement/Winner.text = victor.getName()
	else:
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/WinnerStatement/Winner.text = "Nobody"
	#get remaining units (no bind) getVictorUnits()
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/RemainingUnits/WinnerUnits.text = str(game.getNextBattleVictorUnits(objectID))
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/RemainingUnits/WinnerPower.text = str(victorPower)
	$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/RemainingUnits/LoserPower.text = str(loserPower)
	
	#get captures (no getters in C++)
	var captures = game.getNextBattleCaptures(objectID)
	
	for capture in captures:
		var captureStatement = Label.new()
		captureStatement.horizontal_alignment=HORIZONTAL_ALIGNMENT_CENTER
		captureStatement.text = "Captures " + capture
		$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/FinalResult/Captures.add_child(captureStatement)
	
	var postBattleMessages
	for phase in postPhases:
		postBattleMessages = game.getNextBattleMessages(objectID, phase)
		
		if (len(postBattleMessages) != 0):
			var displayPhase
			displayPhase = preload("res://BattleSpecialistEffects.tscn").instantiate()
			displayPhase.init(p1.getID(), phase, postBattleMessages, p1.getColor(), p2.getColor())
			$VBoxContainer/ScrollContainer/HBoxContainer/VBoxContainer/PostBattle.add_child(displayPhase)
	
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
