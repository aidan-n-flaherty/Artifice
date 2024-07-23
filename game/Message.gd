extends MarginContainer

var message

var maxWidth

# Called when the node enters the scene tree for the first time.
func _ready():
	await get_tree().process_frame

	$VBoxContainer/HBoxContainer/Control/MarginContainer/Label.text = self.message.content

func setMaxWidth(maxWidth):
	self.maxWidth = maxWidth

func init(maxWidth, message, color):
	self.maxWidth = maxWidth
	self.message = message
	
	if message.senderID == GameData.getID():
		$VBoxContainer/HBoxContainer/Control/SelfSent.show()
		$VBoxContainer/HBoxContainer/LSpacer.show()
		$VBoxContainer/HBoxContainer/LSpacer2.show()
	else:
		$VBoxContainer/HBoxContainer/Control/OtherSent.modulate = color
		$VBoxContainer/HBoxContainer/Control/MarginContainer/Label["theme_override_colors/default_color"] = Color(0.9, 0.9, 0.9)
		$VBoxContainer/HBoxContainer/Control/OtherSent.show()
		$VBoxContainer/HBoxContainer/RSpacer.show()
		$VBoxContainer/HBoxContainer/RSpacer2.show()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if $VBoxContainer/MarginContainer/VBoxContainer/TimeContainer/Time.text != str(Utilities.timeToDateStr(message.timestamp)):
		$VBoxContainer/MarginContainer/VBoxContainer/TimeContainer/Time.text = str(Utilities.timeToDateStr(message.timestamp))
	
	var lspace = $VBoxContainer/HBoxContainer/LSpacer.size.x if $VBoxContainer/HBoxContainer/LSpacer.visible else 0
	var rspace = $VBoxContainer/HBoxContainer/RSpacer.size.x if $VBoxContainer/HBoxContainer/RSpacer.visible else 0
	
	if $VBoxContainer/HBoxContainer/Control.size.x >= maxWidth - lspace - rspace:
		$VBoxContainer/HBoxContainer/Control/MarginContainer/Label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		$VBoxContainer/HBoxContainer/Control.size_flags_horizontal = SIZE_EXPAND_FILL

func displayName(display: bool):
	if display:
		$VBoxContainer/MarginContainer/VBoxContainer/Name.text = (await GameData.getUser(message.senderID)).username
		if message.senderID != GameData.getSelfID():
			$VBoxContainer/MarginContainer/VBoxContainer/Name.horizontal_alignment = HORIZONTAL_ALIGNMENT_LEFT
		$VBoxContainer/MarginContainer.show()
		$VBoxContainer/MarginContainer/VBoxContainer/Name.show()

func displayTime(display: bool):
	if display:
		$VBoxContainer/MarginContainer.show()
	$VBoxContainer/MarginContainer/VBoxContainer/TimeContainer.visible = display
