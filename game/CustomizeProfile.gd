extends Control

var user

var userID
var banner
var update = -1
var bannerID 

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func init():
	userID = GameData.getSelfID()
	user = GameData.getSelf()
	
	var bannerID = user.bannerID
	var pC = user.primaryBannerColor
	var sC = user.secondaryBannerColor
	var playerPrimaryColor = Color(pC[0], pC[1], pC[2]) #retrieve from data
	var playerSecondaryColor =  Color(sC[0], sC[1], sC[2])  #retrieve from data
	banner = preload("res://Banner.tscn").instantiate()
	banner.init(bannerID, playerPrimaryColor, playerSecondaryColor)
	$VBoxContainer/HBoxContainer/Panel/MarginContainer/BannerPanel.add_child(banner)
	
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSlider.value = pC[0]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value = pC[1]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSlider.value = pC[2]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSlider.value = sC[0]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value = sC[1]
	$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSlider.value = sC[2]
	
	var bannerShop = preload("res://BannerShop.tscn").instantiate()
	bannerShop.init(userID)
	bannerShop.bannerSelected.connect(selected)
	bannerShop.bannerPurchased.connect(purchased)
	
	$VBoxContainer/BannerShop.add_child(bannerShop)
	

func checkSliders():
	if !user:
		return
	
	if($VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSlider.value != user.primaryBannerColor[0] 
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value != user.primaryBannerColor[1] 
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSlider.value != user.primaryBannerColor[2]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSlider.value != user.secondaryBannerColor[0]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value != user.secondaryBannerColor[1]
	or $VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSlider.value != user.secondaryBannerColor[2]):
		update = 1
		user.primaryBannerColor = [
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/REDS/RedSlider.value,
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/GREENS/GreenSlider.value,
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Primary/BLUES/BlueSlider.value 
		]
		user.secondaryBannerColor = [
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/REDS/RedSlider.value,
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/GREENS/GreenSlider.value,
			$VBoxContainer/BannerColor/VBoxContainer/BannerColorManager/VBoxContainer/Secondary/BLUES/BlueSlider.value
		]
		
		banner.setPrimaryColor(user.primaryBannerColor)
		banner.setSecondaryColor(user.secondaryBannerColor)
		
		return true
	return false

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	checkSliders()
	
	if update > 0:
		update -= delta
		
		if update <= 0:
			GameData.editSelf(user)
			
			self.user = await GameData.getUser(userID)
			
			var bannerID = user.bannerID
			banner = preload("res://Banner.tscn").instantiate()
			banner.init(bannerID, user.primaryBannerColor, user.secondaryBannerColor)
			for child in $VBoxContainer/HBoxContainer/Panel/MarginContainer/BannerPanel.get_children():
				child.queue_free()
			$VBoxContainer/HBoxContainer/Panel/MarginContainer/BannerPanel.add_child(banner)


func selected(bannerID: int):
	update = 1
	user.bannerID = bannerID
	
func purchased(bannerID: int):
	update = 1
	#idk
	
func _on_back_button_pressed() -> void:
	GameData.previous()
