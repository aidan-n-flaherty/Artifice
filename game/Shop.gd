extends Control

signal deselectShop

var gameID

var game

var specialists

var subMenu = 0

var combatSpecialists = []
var combatSpecialistsNames = ["Infiltrator", "Lieutenant", "Thief", "Martyr"]

var counterSpecialists = []
var counterSpecialistsNames = ["Assassin", "Revered_Elder", "Diplomat", "Hypnotist"]

var defenseSpecialists = []
var defenseSpecialistsNames = ["Princess", "Inspector", "Saboteur", "Sentry"]

var transitSpecialists = []
var transitSpecialistsNames = ["Navigator", "Lieutenant", "Smuggler", "Helmsman"]

var sonarSpecialists = []
var sonarSpecialistsNames = ["Intelligence_Officer", "Princess"]

var unitSpecialists = []
var unitSpecialistsNames = ["Foreman", "Tinkerer"]

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	game = GameData.getGame(gameID)
	self.gameID = gameID
	specialists = game.getShopOptions()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".hide()
	
	for specialistNum in specialists:
		var item = ResourceLoader.load("res://SpecialistOffer.tscn").instantiate()
		item.init(specialistNum, gameID, true)
		item.hired.connect(hired)
		var specialistName = game.getSpecialistName(specialistNum)
		
		if specialistName in combatSpecialistsNames:
			combatSpecialists.append(item)
			
		if specialistName in counterSpecialistsNames:
			counterSpecialists.append(item)
		
		if specialistName in defenseSpecialistsNames:
			defenseSpecialists.append(item)
			
		if specialistName in transitSpecialistsNames:
			transitSpecialists.append(item)
			
		if specialistName in sonarSpecialistsNames:
			sonarSpecialists.append(item)
			
		if specialistName in unitSpecialistsNames:
			unitSpecialists.append(item)
		
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var hires = game.getHires()
	$VBoxContainer/Label.text = "You can hire " + str(hires) + " specialist" + ("" if hires == 1 else "s")
	$VBoxContainer/HBoxContainer/MarginContainer/MarginContainer/Label.text = str(hires)

func hired():
	emit_signal("deselectShop")


func _on_combat_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in combatSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 1

func _on_counter_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in counterSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 2


func _on_defense_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in defenseSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 3


func _on_transit_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in transitSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 4

func _on_sonar_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in sonarSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 5


func _on_unit_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".show()
	
	for specialist in unitSpecialists:
		$VBoxContainer/ScrollContainer/Stack.add_child(specialist)
		
	subMenu = 6


func _on_back_button_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".hide()
	
	match subMenu:
		1:
			for specialist in combatSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		2:
			for specialist in counterSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		3:
			for specialist in defenseSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		4:
			for specialist in transitSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		5:
			for specialist in sonarSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		6:
			for specialist in unitSpecialists:
				$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
	
	subMenu = 0
