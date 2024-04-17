extends Panel


# Called when the node enters the scene tree for the first time.
func _ready():
	get_viewport().connect("size_changed", resize)
	
	resize()

func resize():
	self.material.set_shader_parameter("screenWidth", get_viewport().size.x)
	self.material.set_shader_parameter("screenHeight", get_viewport().size.y)
