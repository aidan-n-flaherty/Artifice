extends MarginContainer

var bannerID: int

# Called when the node enters the scene tree for the first time.
func init(bannerID: int, primaryColor, secondaryColor):
	setPrimaryColor(primaryColor)
	setSecondaryColor(secondaryColor)
	setPattern(bannerID)
	
func setPrimaryColor(arr):
	$Primary.modulate = Color(arr[0]/255.0, arr[1]/255.0, arr[2]/255.0)

func setSecondaryColor(arr):
	$Secondary.modulate = Color(arr[0]/255.0, arr[1]/255.0, arr[2]/255.0)
	
func setPattern(bannerID: int):
	if self.bannerID == bannerID:
		return
		
	self.bannerID = bannerID
	
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
	
	$Primary.get_theme_stylebox("panel").texture = texture
	
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
