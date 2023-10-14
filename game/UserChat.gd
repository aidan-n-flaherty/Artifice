extends MarginContainer

var chat

var participants

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func init(chat):
	self.chat = chat
	
	$MarginContainer/VBoxContainer/Participants.text = ""
	for i in len(chat.participants):
		var user = GameData.getUser(chat.participants[i])
		participants.add(user)
		
		if i > 0:
			$MarginContainer/VBoxContainer/Participants.text += ", "
		if len(chat.participants) >= 2 and i == len(chat.participants) - 1:
			$MarginContainer/VBoxContainer/Participants.text += "and "
		$MarginContainer/VBoxContainer/Participants.text += user.username
	
	
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
