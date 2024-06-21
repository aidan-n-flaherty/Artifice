extends MarginContainer


# Called when the node enters the scene tree for the first time.
func init(bannerID: int, primaryColor: Color, secondaryColor: Color):
	$Secondary.modulate = secondaryColor
	$Primary.modulate = primaryColor
	
	# Load the appropriate texture based on bannerID
	var texture_path = ""
	match bannerID:
		1:
			texture_path = "res://resources/banner/horizontalstripe.png"
		2: 
			texture_path = "res://resources/banner/verticalstripe.png"
		3:
			texture_path = "res://resources/banner/diagonalstripes.png"
		4:
			texture_path = "res://resources/banner/checkerboard.png"
		_:
			texture_path = "res://resources/banner/plain.png"
	
	var texture = load(texture_path)
	if not texture:
		print("Error: Texture failed to load from path: " + texture_path)
		return
	
	# Create and configure the StyleBoxTexture
	var style_box_texture = StyleBoxTexture.new()
	style_box_texture.texture = texture
	style_box_texture.axis_stretch_horizontal = StyleBoxTexture.AXIS_STRETCH_MODE_TILE
	style_box_texture.axis_stretch_vertical = StyleBoxTexture.AXIS_STRETCH_MODE_TILE
	
	# Apply the StyleBoxTexture to the Primary node
	$Primary.add_theme_stylebox_override("PrimaryPattern", style_box_texture)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
