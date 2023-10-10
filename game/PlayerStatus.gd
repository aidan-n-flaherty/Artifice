extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(username, units, capacity, resources, win, outposts, factories, generators, mines):
	$MarginContainer/HBoxContainer/Name.text = str(username)
	$MarginContainer/HBoxContainer/VBox/HBox1/Units_Capacity.text = str(units)
	$MarginContainer/HBoxContainer/VBox/HBox1/Outposts.text = str(outposts)
	$MarginContainer/HBoxContainer/VBox/HBox1/Factories.text = str(factories)
	$MarginContainer/HBoxContainer/VBox/HBox1/Generators.text = str(generators)
	$MarginContainer/HBoxContainer/VBox/HBox1/Mines.text  = str(mines)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
