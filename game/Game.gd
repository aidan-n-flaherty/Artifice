extends Control

var gameID
var game

var viewing

# Called when the node enters the scene tree for the first time.
func _ready():
	game = GameData.getGame(gameID)
	game.connect("addOrder", addOrder)
	
	get_node("Viewport").add_child(game)
	
	viewing = get_node("Viewport")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func setView(node):
	viewing.hide()
	viewing = node
	viewing.show()

func _on_game_pressed():
	setView(get_node("Viewport"))

func _on_chat_pressed():
	setView(get_node("Chat"))

func _on_production_pressed():
	setView(get_node("Viewport"))

func _on_logs_pressed():
	setView(get_node("Viewport"))
	
func addOrder(type, referenceID, timestamp, arguments):
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": type,
		"referenceID": referenceID,
		"timestamp": timestamp + 2,
		"argumentIDs": arguments
	}, {
		"gameID": gameID
	})
	
	print(order)
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), int(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")
