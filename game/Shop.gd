extends Control

signal deselectShop

var gameID

var game

var specialists

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	specialists = game.getShopOptions()
	
	for specialistNum in specialists:
		var item = ResourceLoader.load("res://SpecialistOffer.tscn").instantiate()
		item.init(specialistNum, gameID, true)
		item.hired.connect(hired)
		
		$VBoxContainer/ScrollContainer/Stack.add_child(item)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var hires = game.getHires()
	$VBoxContainer/Label.text = "You can hire " + str(hires) + " specialist" + ("" if hires == 1 else "s")
	$VBoxContainer/HBoxContainer/MarginContainer/MarginContainer/Label.text = str(hires)

func hired():
	emit_signal("deselectShop")
