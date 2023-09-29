extends MarginContainer

var gameID: int

var game: GameInterface

var vessel: VesselNode

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(vessel, gameID):
	self.vessel = vessel
	self.gameID = gameID
	game = GameData.getGame(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

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
