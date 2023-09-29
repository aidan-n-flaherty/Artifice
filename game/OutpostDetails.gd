extends MarginContainer

var gameID: int

var game: GameInterface

var outpost: OutpostNode

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(outpost, gameID):
	self.outpost = outpost
	self.gameID = gameID
	game = GameData.getGame(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if outpost.canProduce():
		$VBoxContainer/HBoxContainer/Spacer1.show()
		$VBoxContainer/HBoxContainer/Jump.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer1.hide()
		$VBoxContainer/HBoxContainer/Jump.hide()
		
	if outpost.canMine():
		$VBoxContainer/HBoxContainer/Mine.show()
	else:
		$VBoxContainer/HBoxContainer/Mine.hide()
		
	if outpost.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '" + outpost.getOriginatingOrderType() + "'"
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()


func _on_jump_pressed():
	var t = game.getNextProductionEvent(outpost.getID())
	if t >= 0:
		game.shiftToTime(t)


func _on_cancel_pressed():
	if outpost.getOriginatingOrder() != -1:
		game.cancelOrder(outpost.getOriginatingOrder())


func _on_mine_pressed():
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": "MINE",
		"referenceID": int(game.getReferenceID()),
		"timestamp": game.getTime(),
		"argumentIDs": [outpost.getID()]
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")
