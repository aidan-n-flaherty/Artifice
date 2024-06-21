extends Control

signal deselectShop

var gameID

var game

var specialists

var combatSpecialists = []
var combatSpecialistsNames = ["Infiltrator", "Lieutenant", "Thief", "Martyr"]
var combatFlag = false 

var counterSpecialists = []
var counterSpecialistsNames = ["Assassin", "Revered_Elder", "Diplomat", "Hypnotist"]
var counterFlag = false 

var defenseSpecialists = []
var defenseSpecialistsNames = ["Princess", "Inspector", "Saboteur", "Sentry"]
var defenseFlag = false 

var transitSpecialists = []
var transitSpecialistsNames = ["Navigator", "Lieutenant", "Smuggler", "Helmsman"]
var transitFlag = false 

var sonarSpecialists = []
var sonarSpecialistsNames = ["Intelligence_Officer", "Princess"]
var sonarFlag = false 

var unitSpecialists = []
var unitSpecialistsNames = ["Foreman", "Tinkerer"]
var unitFlag = false 

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
		
	combatFlag = true

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
		
	counterFlag = true


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
		
	defenseFlag = true


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
		
	transitFlag = true


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
		
	sonarFlag = true


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
		
	unitFlag = true


func _on_back_button_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Back Button".hide()
	
	if combatFlag:
		for specialist in combatSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		combatFlag = false
		
	if defenseFlag:
		for specialist in defenseSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		defenseFlag = false

	if counterFlag:
		for specialist in counterSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		counterFlag = false
		
	if transitFlag:
		for specialist in transitSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		transitFlag = false

	if sonarFlag:
		for specialist in sonarSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		sonarFlag = false
		
	if unitFlag:
		for specialist in unitSpecialists:
			$VBoxContainer/ScrollContainer/Stack.remove_child(specialist)
		unitFlag = false
