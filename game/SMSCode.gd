extends MarginContainer

var phoneNumber: String

var share = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func setPhoneNumber(number: String):
	self.phoneNumber = number
	
	
func setShare(share: bool):
	self.share = share
	
	if share:
		$MarginContainer/VBoxContainer/Label.text = "This phone number is already associated with an account.\nPress 'share account' in settings, and input the code here."

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	$MarginContainer/VBoxContainer/HBoxContainer/Continue.disabled = len($MarginContainer/VBoxContainer/Code.getText()) != 7


func _on_continue_pressed():
	if len($MarginContainer/VBoxContainer/Code.getText()) == 7:
		if share:
			if not await GameData.getShare($MarginContainer/VBoxContainer/Code.getText()):
				$MarginContainer/VBoxContainer/Label.text = "Invalid code. Try again."
		#else:
			#if not await GameData.signup(phoneNumber, $MarginContainer/VBoxContainer/Code.getText()):
				#$MarginContainer/VBoxContainer/Label.text = "Invalid code. Try again."

func _on_cancel_pressed():
	GameData.goto_scene("res://Signup.tscn")
