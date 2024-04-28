extends MarginContainer

var gameID: int

var game: GameInterface

var vessel: VesselNode

var vesselID: int

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
	
	var arrival = game.getNextArrivalEvent(vessel.getID())
	if arrival >= 0:
		$VBoxContainer/HBoxContainer2/VBoxContainer/Arrival.text = "Arrives in " + Utilities.timeToStr(game.getNextArrivalEvent(vessel.getID()) - Time.get_unix_time_from_system())
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
	
	if owns and vessel.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '" + vessel.getOriginatingOrderType() + "'"
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
	
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
	GameData.addOrder(gameID, "RELEASE", int(game.getReferenceID()), game.getTime(), [vessel.getID()])


func _on_battle_forecast_pressed():
	emit_signal("battleForecastToggle", vessel)
