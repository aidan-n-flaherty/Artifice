extends Control

signal deselectShop

var userID

var user

var banners

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(userID):
	user = await GameData.getUser(userID)
	self.userID = userID
	
	banners = 5 #currently, find other way to add
	
	for bannerID in range(banners):
		var item = ResourceLoader.load("res://BannerOffer.tscn").instantiate()
		item.init(bannerID, userID, false, true)
		item.selected.connect(selected)
		item.purchased.connect(purchased)
		
		$VBoxContainer/ScrollContainer/Stack.add_child(item)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var banners = 5 #user.getOwnedBanners or game.getBanners or something
	$VBoxContainer/Label.text = "You own " + str(banners) + " banner" + ("" if banners == 1 else "s")
	$VBoxContainer/HBoxContainer/MarginContainer/MarginContainer/Label.text = str(banners)

func selected():
	pass
	
func purchased():
	pass
