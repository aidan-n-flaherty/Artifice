extends Control

var user

var userID
var banner
var update = -1
var bannerID 


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func init(userID: int):
	$VBoxContainer/Title/BannerPreview.text = "[center]Banner Preview[/center]"
	self.userID = userID
	self.user = await GameData.getUser(userID)
	user = self.user
	
	if(!user):
		return
	
	var bannerID = user.bannerID
	var pC = user.primaryBannerColor
	var sC = user.secondaryBannerColor
	var playerPrimaryColor = Color(pC[0], pC[1], pC[2]) #retrieve from data
	var playerSecondaryColor =  Color(sC[0], sC[1], sC[2])  #retrieve from data
	banner = preload("res://Banner.tscn").instantiate()
	banner.init(bannerID, playerPrimaryColor, playerSecondaryColor)
	$VBoxContainer/HBoxContainer/Banner.add_child(banner)
	
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSLider.value = pC[0]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value = pC[1]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSLider.value = pC[2]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSLider.value = sC[0]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value = sC[1]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSLider.value = sC[2]
	
	var bannerShop = preload("res://BannerShop.tscn").instantiate()
	bannerShop.init(userID)
	$VBoxContainer/BannerShop.add_child(bannerShop)
	

func checkSliders():
	user = await GameData.getUser(userID)
	if(!user):
		return false
		
	if($VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSLider.value != user.primaryBannerColor[0] 
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value != user.primaryBannerColor[1] 
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSLider.value != user.primaryBannerColor[2]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSLider.value != user.secondaryBannerColor[0]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value != user.secondaryBannerColor[1]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSLider.value != user.secondaryBannerColor[2]):
		update = 1
		user.primaryBannerColor[0] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSLider.value
		user.primaryBannerColor[1] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value
		user.primaryBannerColor[2] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSLider.value 
		user.secondaryBannerColor[0] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSLider.value 
		user.secondaryBannerColor[1] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value 
		user.secondaryBannerColor[2] = $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSLider.value
		return true
	return false

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	checkSliders()
	
	if update > 0:
		update -= delta
		
		if update <= 0:
			GameData.editSelf(user)


func _on_back_button_pressed() -> void:
	pass # Replace with function body.
