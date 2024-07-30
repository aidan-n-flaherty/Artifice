extends MarginContainer

var units = 0
var capacity = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(game, player, username, dailyProduction, units, capacity, winCount, winTarget, largest, outposts, factories, generators, mines, color, bannerID):
	$MarginContainer/HBoxContainer/VBoxName/Name.text = str(username)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/DailyProduction.text = "+%d / day" % dailyProduction
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Units.text = str(units)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Outposts.text = "= " + str(outposts)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Factories.text = str(factories)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Generators.text = str(generators)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/Mines.text  = str(mines)
	
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Units.text = str(units)
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox2/Capacity.text = str(capacity)
	
	$MarginContainer/HBoxContainer/VBoxName/MarginContainer.visible = winTarget != -1
	
	if winTarget != -1:
		$MarginContainer/HBoxContainer/VBoxName/MarginContainer/WinProgress.value = winCount
		$MarginContainer/HBoxContainer/VBoxName/MarginContainer/WinProgress.max_value = winTarget
		$MarginContainer/HBoxContainer/VBoxName/MarginContainer/MarginContainer/Label.text = str(winCount) + " of " + str(winTarget)
	
	self.units = units
	$MarginContainer/HBoxContainer/MarginContainer/Units.max_value = largest
	$MarginContainer/HBoxContainer/MarginContainer/Units.modulate = color

	self.capacity = capacity
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.max_value = largest
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.modulate = color
	
	var banner = $MarginContainer/HBoxContainer/MarginContainer/Units/Banner
	banner.init(bannerID, [color.r * 200, color.g * 200, color.b * 200], [200, 200, 200])
	
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/factoryIcon.modulate = color
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/gensIcon.modulate = color
	$MarginContainer/HBoxContainer/MarginContainer/MarginContainer/VBox/HBox1/minesIcon.modulate = color
	
	if(game.getUserGameID() != player.getID()):
		$Panel.modulate = Color(0.0, 0.0, 0.0)
	else:
		$Panel.modulate = Color(1.0, 1.0, 1.0)
	
	if game.getNumTeams() > 1:
		$MarginContainer/HBoxContainer/VBoxName/Team.show()
		$MarginContainer/HBoxContainer/VBoxName/Team.text = "Team #" + str(1 + player.getTeamID())
	else:
		$MarginContainer/HBoxContainer/VBoxName/Team.hide()
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$MarginContainer/HBoxContainer/MarginContainer/Units.value = 0.9 * $MarginContainer/HBoxContainer/MarginContainer/Units.value + 0.1 * units
	$MarginContainer/HBoxContainer/MarginContainer/Capacity.value = 0.9 * $MarginContainer/HBoxContainer/MarginContainer/Capacity.value + 0.1 * capacity
	
