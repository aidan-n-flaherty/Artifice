extends Control

var gameID

var game

var playerOrders = []


func update():
	var orderIDs = game.getOrderIDs()
	var invalidIDs = game.getInvalidOrderIDs()
	
	while len(playerOrders) < len(orderIDs):
		playerOrders.append(null)
	
	while len(orderIDs) < len(playerOrders):
		var order = playerOrders.pop_back()
		$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.remove_child(order)
		order.queue_free()
		
	for i in range(0, len(orderIDs)):
		var orderID = orderIDs[i]
		var order
		
		if playerOrders[i]:
			order = playerOrders[i]
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.move_child(order, i)
		else:
			order = preload("res://Order.tscn").instantiate()
			order.cancel.connect(cancelOrder)
			order.shiftToTime.connect(shiftToTime)
			playerOrders[i] = order
			$MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer.add_child(order)
		
		order.init(orderID, game.getOrderTimestamp(orderID), game.getOrderDescription(orderID), invalidIDs.has(orderID), game.canUndoOrder(orderID))
	
# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	update()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if visible:
		update()

func cancelOrder(orderID: int):
	GameData.cancelOrder(gameID, orderID)

func shiftToTime(timestamp: float):
	game.shiftToTime(timestamp)
