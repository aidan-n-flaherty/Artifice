extends MarginContainer

signal hired

var specialistNum

var gameID

var game

var specialistName

var specialistDescription

var promotionOptions

var promotionViews = {}

var hireable: bool

# Called when the node enters the scene tree for the first time.
func _ready():
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Title.text = specialistName.replace("_", " ")
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Description.text = specialistDescription
	
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/MarginContainer/MarginContainer/Image.texture = ResourceLoader.load("res://resources/specialistIcons/" + specialistName + ".png")
	
func init(specialistNum: int, gameID: int, hireable=true):
	self.game = GameData.getGame(gameID)
	self.gameID = gameID
	self.hireable = hireable
	
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Hire.visible = hireable
	
	self.specialistNum = specialistNum
	specialistName = game.getSpecialistName(specialistNum)
	var amount = game.getSpecialistHireAmount(specialistNum)
	if amount > 1:
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Multiplier.text = "x%d" % amount
	
	specialistDescription = game.getSpecialistDescription(specialistNum)
	promotionOptions = game.getPromotionOptions(specialistNum)
	
	for child in $VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.get_children():
		if child.name != "Label":
			$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.remove_child(child)
			child.queue_free()
	
	for option in promotionOptions:
		var button = Button.new()
		button.pressed.connect(viewSpecialist.bind(option))
		button.text = game.getSpecialistName(option)
		
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.show()
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.add_child(button)

func viewSpecialist(specialistNum: int):
	if not self.promotionViews.has(specialistNum):
		var node = ResourceLoader.load("res://SpecialistOffer.tscn").instantiate()
		node.init(specialistNum, gameID, false)
	
		$VBoxContainer/PromotionDisplays/VBoxContainer.add_child(node)
	
		self.promotionViews[specialistNum] = node
	else:
		$VBoxContainer/PromotionDisplays/VBoxContainer.remove_child(self.promotionViews[specialistNum])
		self.promotionViews.erase(specialistNum)

func _process(delta):
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Hire.visible = hireable and game.canHire()

func _on_button_pressed():
	GameData.addOrder(gameID, "HIRE", int(game.getReferenceID()), game.getTime(), [int(specialistNum)])
	
	emit_signal("hired")
