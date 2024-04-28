extends MarginContainer

var units = 0
var capacity = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(game, player, username, units, capacity, resources, largest, win, outposts, factories, generators, mines, color):
	$MarginContainer/HBoxContainer/VBoxName/Name.text = str(username)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Units.text = str(units)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Outposts.text = "= " + str(outposts)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Factories.text = str(factories)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Generators.text = str(generators)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Mines.text  = str(mines)
	
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Units.text = str(units)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Capacity.text = str(capacity)
	
	$MarginContainer/HBoxContainer/VBoxName/ResourcesProgress.value = resources
	$MarginContainer/HBoxContainer/VBoxName/ResourcesProgress.max_value = win
	
	self.units = units
	$MarginContainer/HBoxContainer/MarginContainer/Units.max_value = largest
	$MarginContainer/HBoxContainer/MarginContainer/Units.modulate = color

	self.capacity = capacity
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.max_value = largest
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.modulate = color
	
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/factoryIcon.modulate = color
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/gensIcon.modulate = color
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/minesIcon.modulate = color
	
	if(game.getUserGameID() != player.getID()):
		$Panel.modulate = Color(0.5, 0.5, 0.5)
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$MarginContainer/HBoxContainer/MarginContainer/Units.value = 0.9 * $MarginContainer/HBoxContainer/MarginContainer/Units.value + 0.1 * units
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.value = 0.9 * $MarginContainer/HBoxContainer/MarginContainer/Capacity.value + 0.1 * capacity
	
