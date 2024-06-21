extends Control

var userID = null

# Called when the node enters the scene tree for the first time.
func _ready():
	if $MarginContainer/VBoxContainer/ScrollContainer/VBoxContainer/Basic/Grid/TeamButtons == 2:
		print("ITS WORKING")
	$MarginContainer/HBoxContainer/TeamSelecter.add_item("No Team")
	$MarginContainer/HBoxContainer/TeamSelecter.add_item("Team 1 claus")
	#while $MarginContainer/HBoxContainer/TeamSelecter.selected == 0: 
		#print("working")
	#while $MarginContainer/HBoxContainer/TeamSelecter.selected == -1:
		#print("optoin 2")
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
	pass


func _on_view_pressed():
	if userID:
		GameData.viewUser(userID)
