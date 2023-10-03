extends MarginContainer

var specialistID: int

var specialistNum: int

var promotionOptions: PackedInt32Array

var gameID: int

var game: GameInterface

var specialistName: String

var specialistDescription: String

# Called when the node enters the scene tree for the first time.
func _ready():
	pass
	
func init(specialistID, gameID):
	self.game = GameData.getGame(gameID)
	self.gameID = gameID
	
	self.specialistID = specialistID
	promotionOptions = game.getPromotionOptions(specialistID)
	specialistNum = game.getSpecialistType(specialistID)
	specialistName = game.getSpecialistName(specialistNum)
	specialistDescription = game.getSpecialistDescription(specialistNum)
	
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Title.text = specialistName.replace("_", " ")
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Description.text = specialistDescription
	
	$MarginContainer/HBoxContainer/MarginContainer/Image.texture = ResourceLoader.load("res://resources/specialistIcons/" + specialistName + ".png")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if game.getSpecialistType(specialistID) != specialistNum:
		init(specialistID, gameID)
	
	if promotionOptions[0] == 0 or not game.canHire():
		$MarginContainer/HBoxContainer/MarginContainer/Button.hide()
	else:
		$MarginContainer/HBoxContainer/MarginContainer/Button.show()

func _on_button_pressed():
	GameData.addOrder(gameID, "PROMOTE", int(game.getReferenceID()), game.getTime(), [int(specialistID), promotionOptions[0]])
