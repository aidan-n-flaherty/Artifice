extends MarginContainer

var message

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(message):
	self.message = message
	
	$HBoxContainer/Control/MarginContainer/Label.text = message.content
	
	if message.senderID == GameData.getID():
		$HBoxContainer/Control/SelfSent.show()
		$HBoxContainer/LSpacer.show()
	else:
		$HBoxContainer/Control/OtherSent.show()
		$HBoxContainer/RSpacer.show()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
