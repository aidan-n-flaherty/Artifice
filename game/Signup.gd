extends MarginContainer


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_continue_pressed():
	$MarginContainer/VBoxContainer/Continue.disabled = true
	var number = $MarginContainer/VBoxContainer/HBoxContainer/CountryCode.text + $MarginContainer/VBoxContainer/HBoxContainer/PhoneNumber.text
	var result = ""
	
	for i in range(len(number)):
		if number.unicode_at(i) >= 48 and number.unicode_at(i) <= 57:
			result += number[i]
	
	if len(result) > 5:
		var response = await GameData.requestCode(result)
		if response:
			var node = preload("res://SMSCode.tscn").instantiate()
			if response is Dictionary and response.has("rejectionReason") and response["rejectionReason"] == "used":
				node.setShare(true)
			
			node.setPhoneNumber(result)
		
			GameData.goto_node(node)
		else:
			$MarginContainer/VBoxContainer/Label.text = "Invalid phone number."
	
	$MarginContainer/VBoxContainer/Continue.disabled = false


func _on_phone_number_text_changed(new_text):
	$MarginContainer/VBoxContainer/Continue.disabled = len($MarginContainer/VBoxContainer/HBoxContainer/PhoneNumber.text) < 5 or len($MarginContainer/VBoxContainer/HBoxContainer/CountryCode.text) == 0
