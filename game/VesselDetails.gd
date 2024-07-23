extends MarginContainer

var gameID: int

var game: GameInterface

var vessel: VesselNode

var vesselID: int

var submittedChange = false

var oldUnits = null

var specialistOptions = {}

signal battleForecastToggle(vessel)

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(vessel, gameID):
	self.vessel = vessel
	self.vesselID = vessel.getID()
	self.gameID = gameID
	game = GameData.getGame(gameID)
	
	$VBoxContainer/HBoxContainer2/Units.text = str(vessel.getUnits())

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not vessel:
		if game.getNode(vesselID):
			vessel = game.getNode(vesselID)
		else:
			$VBoxContainer/HBoxContainer2/VBoxContainer/Arrival.text = ""
			return
	
	var unitsChanged = false
	
	if oldUnits != vessel.getUnits():
		unitsChanged = true
		oldUnits = vessel.getUnits()
	
	if submittedChange and unitsChanged:
		$VBoxContainer/HBoxContainer2/Units.release_focus()
		submittedChange = false
	
	if not $VBoxContainer/HBoxContainer2/Units.has_focus():
		$VBoxContainer/HBoxContainer2/Units.text = str(vessel.getUnits())
	
	var speed = vessel.getSpeed() * (60.0 * 60.0) / 2.0 / game.getSimulationSpeed()
	if abs(speed - 1.0) > 0.001:
		$VBoxContainer/HBoxContainer2/VBoxContainer/Speed.show()
		$VBoxContainer/HBoxContainer2/VBoxContainer/Speed.text = ("%0.1fx speed" % speed) if ("%0.2f" % speed).ends_with("0") else ("%0.2fx speed" % speed)
	else:
		$VBoxContainer/HBoxContainer2/VBoxContainer/Speed.hide()
		
	$VBoxContainer/HBoxContainer2/VBoxContainer/GlobalDisabled.visible = vessel.getGlobalDisabled()
	
	var arrival = game.getNextArrivalEvent(vessel.getID())
	if arrival >= 0:
		$VBoxContainer/HBoxContainer2/VBoxContainer/Arrival.text = "Arrives in " + Utilities.timeToStr(arrival - game.getTime())
		$VBoxContainer/HBoxContainer/Spacer1.show()
		$VBoxContainer/HBoxContainer/Jump.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer1.hide()
		$VBoxContainer/HBoxContainer/Jump.hide()
		$VBoxContainer/HBoxContainer2/VBoxContainer/Arrival.text = "This vessel will never reach its target"
	
	var battle = game.getNextBattleEvent(vessel.getID())
	if battle and game.canViewNextBattle(vessel.getID()):
		$VBoxContainer/HBoxContainer/Spacer3.show()
		$VBoxContainer/HBoxContainer/BattleForecast.show()
	else: 
		$VBoxContainer/HBoxContainer/Spacer3.hide()
		$VBoxContainer/HBoxContainer/BattleForecast.hide()
	
	var owns = game.ownsObj(vessel.getID())
		
	if owns and game.canRetreat(vessel.getID()):
		$VBoxContainer/HBoxContainer/Spacer4.show()
		$VBoxContainer/HBoxContainer/Retreat.show()
	else: 
		$VBoxContainer/HBoxContainer/Spacer4.hide()
		$VBoxContainer/HBoxContainer/Retreat.hide()
	
	$VBoxContainer/HBoxContainer/Gift.visible = owns and game.canGift(vessel.getID())
	
	if owns and vessel.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '" + vessel.getOriginatingOrderType() + "'"
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
		$VBoxContainer/HBoxContainer2/AlterOrder.show()
		$VBoxContainer/HBoxContainer2/Units.editable = true
		$VBoxContainer/HBoxContainer2/Units.virtual_keyboard_enabled = true
		$VBoxContainer/HBoxContainer2/Units.focus_mode = FOCUS_ALL

		if vessel.canUndoSource():
			var possibleSpecialists = game.possibleSpecialists(vessel.getSourceOrder())
			var currentSpecialists = vessel.getSpecialistIDs()
			
			$VBoxContainer/HBoxContainer2/Specialists.visible = len(possibleSpecialists) > 0
			
			var possibleSpecialistArr = []
			
			for specialistID in possibleSpecialists:
				if specialistID in currentSpecialists:
					possibleSpecialistArr.push_back(specialistID)
			
			for specialistID in possibleSpecialists:
				if not specialistID in currentSpecialists:
					possibleSpecialistArr.push_back(specialistID)
			
			for specialistID in possibleSpecialistArr:
				var selected = specialistID in currentSpecialists
				var specialistType = game.getSpecialistTypeBeforeOrder(specialistID, vessel.getSourceOrder())
				var specialistName = game.getSpecialistName(specialistType)
				
				var specialist = null
				if specialistOptions.has(specialistID):
					specialist = specialistOptions[specialistID]
				else:
					specialist = preload("res://VesselSpecialist.tscn").instantiate()
					specialist.selected.connect(toggledSpecialist)
					specialistOptions[specialistID] = specialist
					$VBoxContainer/HBoxContainer2/Specialists/ScrollContainer/Specialists.add_child(specialist)
				
				specialist.init(specialistID, specialistName, selected)
			
			for specialistID in specialistOptions.keys():
				if specialistID not in possibleSpecialists:
					$VBoxContainer/HBoxContainer2/Specialists/ScrollContainer/Specialists.remove_child(specialistOptions[specialistID])
					specialistOptions.erase(specialistID)
		else:
			$VBoxContainer/HBoxContainer2/Specialists.hide()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
		$VBoxContainer/HBoxContainer2/AlterOrder.hide()
		$VBoxContainer/HBoxContainer2/Units.editable = false
		$VBoxContainer/HBoxContainer2/Units.virtual_keyboard_enabled = false
		$VBoxContainer/HBoxContainer2/Units.focus_mode = FOCUS_NONE
		$VBoxContainer/HBoxContainer2/Specialists.hide()
	
	get_parent().color = vessel.getColor()
	
	if vessel.getOwnerID() != -1:
		get_parent().playerName = game.getPlayer(vessel.getOwnerID()).getName()
	else:
		get_parent().playerName = "Neutral"

func _on_cancel_pressed():
	GameData.cancelOrder(gameID, vessel.getOriginatingOrder())


func _on_jump_pressed():
	var t = game.getNextArrivalEvent(vessel.getID())
	if t >= 0:
		game.shiftToTime(t)


func _on_gift_pressed():
	GameData.addOrder(gameID, "GIFT", int(game.getReferenceID()), game.getTime(), [vessel.getID()])


func _on_battle_forecast_pressed():
	emit_signal("battleForecastToggle", vessel)


func _on_increment_pressed():
	if vessel.canUndo():
		game.incrementSend(vessel.getSourceOrder())


func _on_decrement_pressed():
	if vessel.canUndo():
		game.decrementSend(vessel.getSourceOrder())


func _on_units_text_submitted(new_text):
	if vessel.canUndo() and new_text.is_valid_int():
		game.alterSend(vessel.getSourceOrder(), new_text.to_int())
		submittedChange = true
	else:
		$VBoxContainer/HBoxContainer2/Units.release_focus()


func _on_units_focus_entered():
	if not $VBoxContainer/HBoxContainer2/Units.editable:
		$VBoxContainer/HBoxContainer2/Units.release_focus()

func _on_units_focus_exited():
	if DisplayServer.has_feature(DisplayServer.FEATURE_VIRTUAL_KEYBOARD):
		DisplayServer.virtual_keyboard_hide()

func toggledSpecialist(specialistID: int):
	if specialistID in vessel.getSpecialistIDs():
		game.removeSpecialist(vessel.getSourceOrder(), specialistID)
	else:
		game.addSpecialist(vessel.getSourceOrder(), specialistID)


func _on_retreat_pressed():
	GameData.addOrder(gameID, "RETREAT", int(game.getReferenceID()), game.getTime(), [vessel.getID()])
