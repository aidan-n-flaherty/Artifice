extends MarginContainer

var specialistNum

var gameID

var game

var specialistName

var specialistDescription

# Called when the node enters the scene tree for the first time.
func _ready():
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Title.text = specialistName.replace("_", " ")
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Description.text = specialistDescription
	
	$MarginContainer/HBoxContainer/MarginContainer/Image.texture = ResourceLoader.load("res://resources/specialistIcons/" + specialistName + ".png")
	
func init(gameID):
	self.game = GameData.getGame(gameID)
	self.gameID = gameID
	
	specialistName = game.getSpecialistName(specialistNum)
	specialistDescription = game.getSpecialistDescription(specialistNum)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_button_pressed():
	print(game.getTime())
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": "HIRE",
		"referenceID": int(game.getReferenceID()),
		"timestamp": int(game.getTime()),
		"argumentIDs": [int(specialistNum)]
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), int(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")
