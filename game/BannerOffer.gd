extends MarginContainer

signal selected(bannerID:int)

signal purchased(bannerID:int)

var bannerID

var userID

var user

var bannerName

var bannerDescription

var selectable: bool

var available: bool

var update = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Title.text = bannerName.replace("_", " ")
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/HBoxContainer/VBoxContainer/Description.text = bannerDescription
	
	
func init(bannerID: int, userID: int, available=false, selectable=true):
	user = await GameData.getUser(userID)
	if(!user): return
	
	self.userID = userID
	self.available = available #default false for now
	self.selectable = selectable #default true for now
	
	print(bannerID)
	var banner = preload("res://Banner.tscn").instantiate()
	banner.init(bannerID, Color(0,0,0), Color(1,1,1))
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/MarginContainer/MarginContainer/Image.add_child(banner)
	
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Select.visible = selectable
	if(user.bannerID == bannerID):
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Select.disabled = true
	
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Purchase.visible = available
	
	self.bannerID = bannerID
	bannerName = ""
	bannerDescription = ""
	
	match bannerID:
		1:
			#texture_path = "res://resources/banner/horizontalstripe.png"
			bannerName = "Horizontal Stripes"
			bannerDescription = "A basic two-color set of horizontal stripes."
		2: 
			#texture_path = "res://resources/banner/verticalstripe.png"
			bannerName = "Vertical Stripes"
			bannerDescription = "A basic two-color set of vertical stripes."
		3:
			#texture_path = "res://resources/banner/diagonalstripes.png"
			bannerName = "Diagonal Stripes"
			bannerDescription = "A basic two-color set of diagonal (bottom left to top right) stripes."
		4:
			#texture_path = "res://resources/banner/checkerboard.png"
			bannerName = "Checkerboard"
			bannerDescription = "A checkerboard pattern for your banner."
		_:
			#texture_path = "res://resources/banner/plain.png"
			bannerName = "Plain"
			bannerDescription = "A solid-color banner using the primary color selected. (DEFAULT)"
	
	var amount = 1

func _process(delta):
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Select.visible = selectable
	
	user = await GameData.getUser(userID)
	if(!user): return
	
	if(user.bannerID == bannerID):
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Select.disabled = true
	else:
		$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Select.disabled = false
		
	$VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/MarginContainer/Purchase.visible = available
	
	if update > 0:
		update -= delta
		
		if update <= 0:
			#GameData.editSelf(user)
			pass

func _on_select_pressed():
	update = 1
	emit_signal("selected",bannerID)
	
func _on_purchase_pressed():
	#nothin yet
	update = 1
	emit_signal("purchased",bannerID)
