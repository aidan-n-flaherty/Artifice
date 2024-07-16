extends MarginContainer

signal battleForecastToggle(outpost)

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
	if not outpost.isInRadar():
		$VBoxContainer/OutOfRange.show()
		$VBoxContainer/HBoxContainer.hide()
		$VBoxContainer/HBoxContainer2.hide()
	else:
		$VBoxContainer/OutOfRange.hide()
		$VBoxContainer/HBoxContainer.show()
		$VBoxContainer/HBoxContainer2.show()
	

	get_parent().color = outpost.getColor()
	
	if outpost.getOwnerID() != -1:
		get_parent().playerName = game.getPlayer(outpost.getOwnerID()).getName()
	else:
		get_parent().playerName = "Neutral"
	
	$Images/HBoxContainer/Factory.visible = (outpost.isInRadar() or outpost.canViewType()) and outpost.isFactory()
	$Images/HBoxContainer/Generator.visible = (outpost.isInRadar() or outpost.canViewType()) and outpost.isGenerator()
	$Images/HBoxContainer/Mine.visible = (outpost.isInRadar() or outpost.canViewType()) and outpost.isMine()
	
	if not (outpost.isInRadar() or outpost.canViewType() or outpost.isMine()):
		$VBoxContainer/HBoxContainer/Jump.hide()
		$Images/HBoxContainer/Mine.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
		$VBoxContainer/HBoxContainer/BattleForecast.hide()
		$VBoxContainer/Type.text = "Unknown"
		
		return
	elif outpost.isFactory():
		$VBoxContainer/Type.text = "Factory"
		if outpost.getOwnerID() != -1:
			$VBoxContainer/HBoxContainer/Spacer1.show()
			$VBoxContainer/HBoxContainer/Jump.show()
			$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = "+" + str(outpost.getProductionAmount()) + " in " + Utilities.timeToStr(game.getNextProductionEvent(outpost.getID()) - game.getTime())
		else:
			$VBoxContainer/HBoxContainer/Spacer1.hide()
			$VBoxContainer/HBoxContainer/Jump.hide()
			$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = ""
	elif outpost.isMine():
		$VBoxContainer/Type.text = "Mine"
		if outpost.getOwnerID() != -1:
			$VBoxContainer/HBoxContainer/Spacer1.show()
			$VBoxContainer/HBoxContainer/Jump.show()
			$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = "+1 in " + Utilities.timeToStr(game.getNextProductionEvent(outpost.getID()) - game.getTime())
		else:
			$VBoxContainer/HBoxContainer/Spacer1.hide()
			$VBoxContainer/HBoxContainer/Jump.hide()
			$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = ""
	elif outpost.isGenerator():
		$VBoxContainer/Type.text = "Generator"
		$VBoxContainer/HBoxContainer/Spacer1.hide()
		$VBoxContainer/HBoxContainer/Jump.hide()
		$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = "+50 to electrical output"
	else:
		$VBoxContainer/Type.text = "Destroyed"
		$VBoxContainer/HBoxContainer2/VBoxContainer/Production.text = ""
		$VBoxContainer/HBoxContainer/Spacer1.hide()
		$VBoxContainer/HBoxContainer/Jump.hide()
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Mine.hide()
	
	var owns = game.ownsObj(outpost.getID())
	if owns and outpost.canMine():
		$VBoxContainer/HBoxContainer/Mine.show()
		$VBoxContainer/HBoxContainer/Mine.text = "Mine (%d)" % outpost.getMineCost()
	else:
		$VBoxContainer/HBoxContainer/Mine.hide()
	
	if owns and outpost.canUndo():
		$VBoxContainer/HBoxContainer/Cancel.text = "Undo '%s'" % outpost.getOriginatingOrderType()
		$VBoxContainer/HBoxContainer/Spacer2.show()
		$VBoxContainer/HBoxContainer/Cancel.show()
	else:
		$VBoxContainer/HBoxContainer/Spacer2.hide()
		$VBoxContainer/HBoxContainer/Cancel.hide()
	
	var battle = game.getNextBattleEvent(outpost.getID())
	if battle >= 0 and outpost.isInRadar():
		$VBoxContainer/HBoxContainer/Spacer3.show()
		$VBoxContainer/HBoxContainer/BattleForecast.show()
	else: 
		$VBoxContainer/HBoxContainer/Spacer3.hide()
		$VBoxContainer/HBoxContainer/BattleForecast.hide()
	
	$VBoxContainer/HBoxContainer2/Units.text = str(outpost.getUnits())


func _on_jump_pressed():
	var t = game.getNextProductionEvent(outpost.getID())
	if t >= 0:
		game.shiftToTime(t)


func _on_cancel_pressed():
	GameData.removeOrder(gameID, outpost.getOriginatingOrder())


func _on_mine_pressed():
	GameData.addOrder(gameID, "MINE", int(game.getReferenceID()), game.getTime(), [outpost.getID()])


func _on_battle_forecast_pressed():
	emit_signal("battleForecastToggle", outpost)
