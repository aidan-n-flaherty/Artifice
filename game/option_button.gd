extends OptionButton


signal teamSelected(index)
# Called when the node enters the scene tree for the first time.
func _ready():
	pass


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_item_selected(index):
	emit_signal("teamNumberSelected", index+1)
	#GameData.setTeam(GameData.getCurrentGameID(), index+1)
	#emit_signal()
