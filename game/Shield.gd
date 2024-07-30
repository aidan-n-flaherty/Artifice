extends MeshInstance3D

@export var factory: CompressedTexture2D

@export var generator: CompressedTexture2D

@export var mine: CompressedTexture2D

@export var unknown: CompressedTexture2D

var dark = false

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	rotation.y += 0.05 * delta
	$Icon.rotation.y = -rotation.y + PI

func setDark(dark: bool):
	if self.dark != dark:
		self.dark = dark
		get_surface_override_material(8).set_shader_parameter("dark", self.dark)

func showFactory():
	$Icon.texture = factory
	$Icon.visible = true

func showGenerator():
	$Icon.texture = generator
	$Icon.visible = true

func showMine():
	$Icon.texture = mine
	$Icon.visible = true

func showUnknown():
	$Icon.texture = unknown
	$Icon.visible = true

func showDestroyed():
	$Icon.visible = false

func setColor(color: Color):
	if color != $Icon.modulate:
		$Icon.modulate = color
		get_surface_override_material(3).albedo_color = color
