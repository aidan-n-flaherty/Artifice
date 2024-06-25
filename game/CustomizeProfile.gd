extends Control

var user

var userID

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func init(userID: int):
	$VBoxContainer/Title/BannerPreview.text = "[center]Banner Preview[/center]"
	self.userID = userID
	self.user = await GameData.getUser(userID)
	
	if(!user):
		return
	
	var bannerID = user.bannerID
	var pC = user.primaryBannerColor
	var sC = user.secondaryBannerColor
	var playerPrimaryColor = Color(pC[0], pC[1], pC[2]) #retrieve from data
	var playerSecondaryColor =  Color(sC[0], sC[1], sC[2])  #retrieve from data
	var banner
	banner = preload("res://Banner.tscn").instantiate()
	banner.init(bannerID, playerPrimaryColor, playerSecondaryColor)
	$VBoxContainer/Banner.add_child(banner)
	
	var bannerShop = preload("res://BannerShop.tscn").instantiate()
	bannerShop.init(bannerID)
	$VBoxContainer/BannerShop.add_child(bannerShop)
	

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
