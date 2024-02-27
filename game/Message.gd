extends MarginContainer

var message

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(message):
	self.message = message
	
	$VBoxContainer/HBoxContainer/Control/MarginContainer/Label.text = message.content
	
	if message.senderID == GameData.getID():
		$VBoxContainer/HBoxContainer/Control/SelfSent.show()
		$VBoxContainer/HBoxContainer/LSpacer.show()
	else:
		$VBoxContainer/HBoxContainer/Control/MarginContainer/Label["theme_override_colors/default_color"] = Color(0.9, 0.9, 0.9)
		$VBoxContainer/HBoxContainer/Control/OtherSent.show()
		$VBoxContainer/HBoxContainer/RSpacer.show()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if $VBoxContainer/MarginContainer/VBoxContainer/TimeContainer/Time.text != str(Utilities.timeToDateStr(message.timestamp)):
		$VBoxContainer/MarginContainer/VBoxContainer/TimeContainer/Time.text = str(Utilities.timeToDateStr(message.timestamp))

func displayName():
	$VBoxContainer/MarginContainer/VBoxContainer/Name.text = (await GameData.getUser(message.senderID)).username
	if message.senderID != GameData.getID():
		$VBoxContainer/MarginContainer/VBoxContainer/Name.horizontal_alignment = HORIZONTAL_ALIGNMENT_LEFT
	$VBoxContainer/MarginContainer.show()
	$VBoxContainer/MarginContainer/VBoxContainer/Name.show()

func displayTime():
	$VBoxContainer/MarginContainer.show()
	$VBoxContainer/MarginContainer/VBoxContainer/TimeContainer.show()
