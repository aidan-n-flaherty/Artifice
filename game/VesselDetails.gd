extends MarginContainer

var gameID: int

var game: GameInterface

var vessel: VesselNode

signal battleForecastToggle(vessel)

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(vessel, gameID):
	self.vessel = vessel
	self.gameID = gameID
	game = GameData.getGame(gameID)
	
	$VBoxContainer/HBoxContainer2/Units.text = str(vessel.getUnits())

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$VBoxContainer/HBoxContainer2/VBoxContainer/Arrival.text = "Arrives in " + Utilities.timeToStr(game.getNextArrivalEvent(vessel.getID()) - game.getTime())
	
	var owns = game.ownsObj(vessel.getID())
	
	if owns and vessel.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '" + vessel.getOriginatingOrderType() + "'"
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
	
	get_parent().color = vessel.getColor()
	get_parent().playerName = game.getPlayer(vessel.getOwnerID()).getName()

func _on_cancel_pressed():
	print(vessel.getOriginatingOrder())
	if vessel.getOriginatingOrder() != -1:
		game.cancelOrder(vessel.getOriginatingOrder())


func _on_jump_pressed():
	var t = game.getNextArrivalEvent(vessel.getID())
	if t >= 0:
		game.shiftToTime(t)


func _on_gift_pressed():
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": "GIFT",
		"referenceID": int(game.getReferenceID()),
		"timestamp": int(game.getTime()),
		"argumentIDs": [vessel.getID()]
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")


func _on_battle_forecast_pressed():
	emit_signal("battleForecastToggle", vessel)
