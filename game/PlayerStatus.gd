extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(username, units, capacity, resources, largest, win, outposts, factories, generators, mines, color):
	$MarginContainer/HBoxContainer/VBoxName/Name.text = str(username)
	$MarginContainer/HBoxContainer/VBox/HBox1/Units_Capacity.text = str(units)
	$MarginContainer/HBoxContainer/VBox/HBox1/Outposts.text = str(outposts)
	$MarginContainer/HBoxContainer/VBox/HBox1/Factories.text = str(factories)
	$MarginContainer/HBoxContainer/VBox/HBox1/Generators.text = str(generators)
	$MarginContainer/HBoxContainer/VBox/HBox1/Mines.text  = str(mines)
	
	$MarginContainer/HBoxContainer/VBox/HBox2/Control/HBoxContainer/unitbar.text = str(units)
	$MarginContainer/HBoxContainer/VBox/HBox2/Control/HBoxContainer/capabar.text = str(capacity)
	
	$MarginContainer/HBoxContainer/VBoxName/ResourcesProgress.value = resources
	$MarginContainer/HBoxContainer/VBoxName/ResourcesProgress.max_value = win
	
	$MarginContainer/HBoxContainer/VBox/HBox2/Control/Units_of_Capacity.value = units
	$MarginContainer/HBoxContainer/VBox/HBox2/Control/Units_of_Capacity.max_value = largest

	$MarginContainer/HBoxContainer/VBox/HBox2/Control/ProgressBar.value = capacity
	$MarginContainer/HBoxContainer/VBox/HBox2/Control/ProgressBar.max_value = largest
	$ColorRect.color = color
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
