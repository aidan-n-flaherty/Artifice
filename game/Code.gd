extends MarginContainer

var boxes = []

# Called when the node enters the scene tree for the first time.
func _ready():
	boxes.push_back($HBoxContainer/LineEdit1)
	for i in range(6):
		var node = $HBoxContainer/LineEdit1.duplicate()
		$HBoxContainer.add_child(node)
		
		boxes.push_back(node)
	
	var i = 0
	for box in boxes:
		box.connect("text_changed", changed.bind(i))
		box.connect("text_change_rejected", changedOverflow.bind(i))
		box.connect("gui_input", gui_input.bind(i))
		
		i += 1

func changed(text: String, index: int):
	boxes[index].release_focus()
		
	if text != "":
		index += 1
	else:
		index -= 1
	
	if index >= 0 and index < len(boxes):
		boxes[index].grab_focus()
		boxes[index].select_all()

func changedOverflow(text: String, index: int):
	boxes[index].release_focus()
		
	index += 1
	
	if index >= 0 and index < len(boxes):
		boxes[index].grab_focus()
		boxes[index].select_all()
		
		boxes[index].text = text
		
func gui_input(event: InputEvent, index: int):
	if event is InputEventKey:
		if event.pressed and event.keycode == KEY_BACKSPACE:
			boxes[index].release_focus()
			
			index -= 1
			
			if index >= 0 and index < len(boxes):
				boxes[index].grab_focus()
				boxes[index].select_all()

func getText():
	var str = ""
	
	for box in boxes:
		str += box.text
	
	return str

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
