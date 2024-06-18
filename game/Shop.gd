extends Control

signal deselectShop

var gameID

var game

var specialists

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(gameID):
	#game = GameData.getGame(gameID)
	#self.gameID = gameID
	#specialists = game.getShopOptions()
	
	#list of specialists currently in the game:
	#combat:
		#infiltrator - drains shiled of outpost it attacks
		#lieutenant - move 50% faster, when participating in combat kill 5 extra units
		#thief - 15% of enemy units converted to yours when participating in combat
		#martyr - when participating in combat destroy everything in 20% of range of sonar
	#
	#counter-specialists:
		#assassin - kills enemy specialists
		#revered elder - negate all specialist effects
		#hypnoist - take control of captured specialists at outpost
		#diplomat  - any captured specialists within sonar range are freed
	#
	#defense:
		#inspector - while at an outpost shield is always fully charged
		#saboteur - redirect victorious submarines away from outpost
		#sentry - shoot enemy submarines while at outpost
	#
	#transit:
		#navigator - lets submarines be redirected during transit
		#lieutenant - move 50% faster, when participating in combat kill 5 extra units
		#smuggler - travels to outposts you own 3x faster
		#helmsman - movement speed is 2x base speed
	#
	#sonar:
		#princess - increases range of sonar, can replace princess
		#intelligence officer - sonar range of all outposts increased by 25%
	#
	#unit capacity/production:
		#foreman - while at a factory 6 more units are produced
		#tinkerer - maximum unit capacity increased to 3x max shield charge
	
	#for specialistNum in specialists:
		#var item = ResourceLoader.load("res://SpecialistOffer.tscn").instantiate()
		#item.init(specialistNum, gameID, true)
		#item.hired.connect(hired)
		#$VBoxContainer/ScrollContainer/Stack.add_child(item)
		
		#important tool that will help us later
		#$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".hide()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var hires = game.getHires()
	$VBoxContainer/Label.text = "You can hire " + str(hires) + " specialist" + ("" if hires == 1 else "s")
	$VBoxContainer/HBoxContainer/MarginContainer/MarginContainer/Label.text = str(hires)

func hired():
	emit_signal("deselectShop")


func _on_combat_specialist_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".hide()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".hide()
	#for specialistNum in specialists:
		#var item = ResourceLoader.load("res://SpecialistOffer.tscn").instantiate()
		#item.init(specialistNum, gameID, true)
		#item.hired.connect(hired)
		#$VBoxContainer/ScrollContainer/Stack.add_child(item)


func _on_back_button_pressed() -> void:
	$VBoxContainer/ScrollContainer/Stack/"Combat Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Counter Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Defense Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Transit Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Sonar Specialist".show()
	$VBoxContainer/ScrollContainer/Stack/"Unit Specialist".show()
	
	#need to remove 
	$VBoxContainer/ScrollContainer/Stack.remove_child()
