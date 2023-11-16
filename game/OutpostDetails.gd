extends MarginContainer

var gameID: int

var game: GameInterface

var outpost: OutpostNode

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	
func init(outpost, gameID):
	self.outpost = outpost
	self.gameID = gameID
	game = GameData.getGame(gameID)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if outpost.canProduce():
		$VBoxContainer/HBoxContainer/Spacer1.show()
		$VBoxContainer/HBoxContainer/Jump.show()
		
		$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = "+" + str(outpost.getProductionAmount()) + " in " + Utilities.timeToStr(game.getNextProductionEvent(outpost.getID()) - game.getTime())
	else:
		$VBoxContainer/HBoxContainer/Spacer1.hide()
		$VBoxContainer/HBoxContainer/Jump.hide()
		
		$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = "+50 to electrical output"
	
	if outpost.canMine():
		$VBoxContainer/HBoxContainer/Mine.show()
	else:
		$VBoxContainer/HBoxContainer/Mine.hide()
		
	if outpost.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '" + outpost.getOriginatingOrderType() + "'"
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
	
	$VBoxContainer/HBoxContainer2/Units.text = str(outpost.getUnits())


func _on_jump_pressed():
	var t = game.getNextProductionEvent(outpost.getID())
	if t >= 0:
		game.shiftToTime(t)


func _on_cancel_pressed():
	if outpost.getOriginatingOrder() != -1:
		game.cancelOrder(outpost.getOriginatingOrder())


func _on_mine_pressed():
	GameData.addOrder(gameID, "MINE", int(game.getReferenceID()), game.getTime(), [outpost.getID()])
