extends MarginContainer

signal selected(specialistID: int)

var specialistID: int

var specialistName = null

func init(specialistID: int, specialistName: String, selected: bool):
	self.specialistID = specialistID
	if self.specialistName != specialistName:
		self.specialistName = specialistName
		$MarginContainer/TextureRect.texture = ResourceLoader.load("res://resources/specialistIcons/" + specialistName + ".png")
	$Panel.material.set_shader_parameter("inverted", not selected)
	$MarginContainer/TextureRect.material.set_shader_parameter("inverted", not selected)

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_button_pressed():
	emit_signal("selected", specialistID)
