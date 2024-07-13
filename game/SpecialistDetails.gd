extends MarginContainer

signal openShop

var specialistID: int

var basePromotionOptions = []

var specialistNums = []

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
	specialistNum = game.getSpecialistType(specialistID)
	basePromotionOptions = game.getPromotionOptions(specialistNum)
	
	initNum(specialistNum)

func initNum(specialistNum):
	self.specialistNum = specialistNum
	promotionOptions = game.getPromotionOptions(specialistNum)
	specialistName = game.getSpecialistName(specialistNum)
	specialistDescription = game.getSpecialistDescription(specialistNum)
	
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Title.text = specialistName.replace("_", " ")
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Description.text = specialistDescription
	
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/Image.texture = ResourceLoader.load("res://resources/specialistIcons/" + specialistName + ".png")
	
	for child in $MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.get_children():
		if child.name != "Label":
			$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.remove_child(child)
			child.queue_free()
		
	for option in promotionOptions:
		var button = Button.new()
		button.theme = preload("res://resources/themes/DetailTheme.tres")
		button.pressed.connect(viewSpecialist.bind(option))
		button.text = game.getSpecialistName(option)
		
		$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.add_child(button)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var owns = game.ownsSpecialist(specialistID)
	var canPromote = game.canPromote(specialistID)
	
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/PromotionOptions.visible = len(promotionOptions) > 0
	
	if len(specialistNums) > 0:
		$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Back.show()
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Promote.visible = owns and specialistNum in basePromotionOptions and canPromote
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Release.hide()
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Shop.hide()
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Cancel.hide()
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/NextHire.hide()
		return
	else:
		$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Back.hide()
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Promote.hide()
	
	var canHire = game.canHire()
	
	if game.getSpecialistType(specialistID) != specialistNum:
		init(specialistID, gameID)
	
	$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/NextHire.visible = specialistName == "Queen" and owns
	$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/NextHireLabel.visible = specialistName == "Queen" and owns
	if specialistName == "Queen" and owns:
		$MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/NextHireLabel.text = "%d hire%s available, next hire in %s" % [game.getHires(), "s" if game.getHires() != 1 else "", Utilities.timeToStr(game.getNextHireEvent() - game.getTime())]
	
	$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Shop.visible = owns and canHire and specialistName == "Queen"
	
	if owns and game.canUndoSpecialist(specialistID):
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Cancel.text = "Undo '" + game.getSpecialistOriginatingOrderType(specialistID) + "'"
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Cancel.show()
	else:
		$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Cancel.hide()
	
	$MarginContainer/HBoxContainer/MarginContainer/VBoxContainer/Release.visible = not owns and game.canRelease(specialistID)
	
	var player = game.getSpecialistOwner(specialistID)
	
	if player:
		get_parent().color = player.getColor()
		get_parent().playerName = player.getName()

func viewSpecialist(specialistNum: int):
	self.specialistNums.push_back(self.specialistNum)
	
	initNum(specialistNum)

func _on_promote_pressed():
	GameData.addOrder(gameID, "PROMOTE", int(game.getReferenceID()), game.getTime(), [int(specialistID), specialistNum])
	specialistNums = []
	init(specialistID, gameID)

func _on_release_pressed():
	GameData.addOrder(gameID, "RELEASE", int(game.getReferenceID()), game.getTime(), [int(specialistID)])

func _on_shop_pressed():
	emit_signal("openShop")

func _on_cancel_pressed():
	GameData.cancelOrder(gameID, game.getSpecialistOriginatingOrder(specialistID))

func _on_next_hire_pressed():
	var t = game.getNextHireEvent()
	if t >= 0:
		game.shiftToTime(t)

func _on_back_pressed():
	specialistNum = self.specialistNums.pop_back()
	
	print("Number ", specialistNum)
	
	initNum(specialistNum)
