extends Control

var userID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	#first create five teams then disable/re enable them as seen fit. 
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("No Team")
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 1")
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 2")
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 3")
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 4")
	#$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 5")

	pass # Replace with function body.

func init(id, username, color, rating, ranked: bool):
	self.userID = id
	$MarginContainer/HBoxContainer/Name.text = str(username)
	$MarginContainer/HBoxContainer/MarginContainer/Color.modulate = color
	$MarginContainer/HBoxContainer/Name.show()
	$MarginContainer/HBoxContainer/Spacer.hide()
	$MarginContainer/HBoxContainer/MarginContainer/Color.show()
	$MarginContainer/HBoxContainer/Rating.text = str(rating)
	$MarginContainer/HBoxContainer/Rating.visible = ranked
	   

	
	
	#$ColorPanel.show()
	#color.a = 0.5
	#$ColorPanel.modulate = color
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	#if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/'0'.button_pressed() ==true:
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(1, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(2, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(3, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(4, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(5, true)
	#if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons/'2'.button_pressed() == true:
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(0, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(1, false)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(2, false)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(3, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(4, true)
		#$MarginContainer/HBoxContainer/TeamSelecter.set_item_disabled(5, true)
	pass

#
#func _on_view_pressed():
	#if userID:
		#GameData.viewUser(userID)

#
#func _on_Team_Selected(index):
	#if index == 0: # meaning no team is selected
		#print("working 1?")
	#pass # Replace with function body.
