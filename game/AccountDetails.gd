extends MarginContainer

var userID

var user

var isSelf

var update = -1

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func init(userID: int):
	self.userID = userID
	self.isSelf = userID == GameData.getSelfID()
	
	GameData.userChanged.connect(updateUser)
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList/MarginContainer.add_theme_constant_override("margin_right", 0)
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList/MarginContainer.add_theme_constant_override("margin_bottom", 0)
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList/MarginContainer.add_theme_constant_override("margin_top", 0)
	
	if isSelf:
		user = GameData.getSelf()
		
		updateUser(userID)
		
		await GameData.loadSelf()
	else:
		$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.suppressReady()
		$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.initUser(userID)
		await $MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.init()
		await GameData.loadUser(userID)

func updateUser(userID):
	if userID != self.userID:
		return
	
	if isSelf:
		user = GameData.getSelf()
	else:
		user = await GameData.getUser(self.userID)
	
	if(!user): return
	
	$MarginContainer/VBoxContainer/CustomizeProfile.disabled = not isSelf
	
	$MarginContainer/VBoxContainer/CustomizeProfile/MarginContainer/Panel/Banner.init(user.bannerID, user.primaryBannerColor, user.secondaryBannerColor)
	
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/TestGame.visible = isSelf
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Restore.visible = isSelf
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Report.visible = not isSelf
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Block.visible = not isSelf
	$MarginContainer/VBoxContainer/GridContainer/UsernameText.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/Username.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/UsernameText.editable = isSelf
	$MarginContainer/VBoxContainer/GridContainer/UsernameText.text = user.username
	$MarginContainer/VBoxContainer/CustomizeProfile/MarginContainer/MarginContainer/VBoxContainer/Username.text = user.username
	$MarginContainer/VBoxContainer/CustomizeProfile/MarginContainer/MarginContainer/VBoxContainer/Rating.text = "Rank #" + str(user.userStats.placement + 1)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/VictoryContainer/MarginContainer/HBoxContainer/Victories.text = str(user.userStats.gamesWon)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/DefeatsContainer/MarginContainer/HBoxContainer/Defeats.text = str(user.userStats.gamesLost)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/TotalGamesContainer/MarginContainer/HBoxContainer/TotalGames.text = str(user.userStats.gamesPlayed)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/AverageRankContainer/MarginContainer/HBoxContainer/AverageRank.text = "N/A" if user.userStats.gamesPlayed == 0 else "%d%%" % int(user.userStats.averageRank * 100.0 / user.userStats.gamesPlayed)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/RatingContainer/MarginContainer/HBoxContainer/Rating.text = str(user.userStats.rating)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/PlacementContainer/MarginContainer/HBoxContainer/Placement.text = "#" + str(user.userStats.placement + 1)
	
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Gold/MarginContainer/HBoxContainer/Gold.text = str(user.userStats.gold)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Silver/MarginContainer/HBoxContainer/Silver.text = str(user.userStats.silver)
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Bronze/MarginContainer/HBoxContainer/Bronze.text = str(user.userStats.bronze)
	
	$MarginContainer/VBoxContainer/GridContainer/PushNotifications.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/PushNotificationButtons.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/PushNotificationButtons/PushOn.set_pressed_no_signal(user.pushEnabled)
	$MarginContainer/VBoxContainer/GridContainer/PushNotificationButtons/PushOff.set_pressed_no_signal(not user.pushEnabled)
	
	$MarginContainer/VBoxContainer/GridContainer/ChatPushNotifications.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons/PushOn.set_pressed_no_signal(user.chatPushEnabled)
	$MarginContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons/PushOff.set_pressed_no_signal(not user.chatPushEnabled)
	
	var settings = GameData.loadLocalSettings()
	$MarginContainer/VBoxContainer/GridContainer/Graphics.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/GraphicsButtons.visible = isSelf
	$MarginContainer/VBoxContainer/GridContainer/GraphicsButtons/PushOn.set_pressed_no_signal(not settings.has("graphics") or settings["graphics"] == "advanced")
	$MarginContainer/VBoxContainer/GridContainer/GraphicsButtons/PushOff.set_pressed_no_signal(settings.has("graphics") and settings["graphics"] == "simple")
	
	$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Delete.visible = isSelf
	
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Leaderboard.visible = isSelf
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Spacer.visible = isSelf
	
	if isSelf:
		var users = await GameData.getRankings()
		
		for child in $MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings/VBoxContainer.get_children():
			child.queue_free()
		
		var i = 1
		for player in users:
			var node = preload("res://RankingView.tscn").instantiate()
			node.init(player.id, player.username, player.userStats.rating, i)
			$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings/VBoxContainer.add_child(node)
			
			i += 1


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if update > 0:
		update -= delta
		
		if update <= 0:
			GameData.editSelf(user)
	

func _on_username_text_text_changed(new_text):
	if len(new_text) < 3 or len(new_text) > 20:
		$MarginContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.FIREBRICK
		return
	else:
		$MarginContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.WHITE
	user.username = new_text
	
	update = 1

func _exit_tree():
	if update > 0:
		update = -1
		GameData.editSelf(user)


func _on_push_on_toggled(toggled_on):
	if toggled_on and not user.pushEnabled:
		user.pushEnabled = true
		
		update = 1


func _on_push_off_toggled(toggled_on):
	if toggled_on and user.pushEnabled:
		user.pushEnabled = false
		
		update = 1


func _on_chat_push_on_toggled(toggled_on):
	if toggled_on and not user.chatPushEnabled:
		user.chatPushEnabled = true
		
		update = 1


func _on_chat_push_off_toggled(toggled_on):
	if toggled_on and user.chatPushEnabled:
		user.chatPushEnabled = false
		
		update = 1


func _on_confirm_pressed():
	if not await GameData.deleteAccount():
		$ConfirmationDialog.hide()


func _on_cancel_pressed():
	$ConfirmationDialog.hide()


func _on_delete_pressed():
	$ConfirmationDialog.show()


func _on_report_pressed():
	if await GameData.reportUser(self.userID, "unspecified"):
		$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Report.visible = false


func _on_restore_pressed():
	var code = await GameData.startShare()
	
	if code:
		var codeStr = code["code"]
		$CodeDialog/Dialog/MarginContainer/VBoxContainer/Code.text = codeStr
		$CodeDialog.show()


func _on_done_pressed():
	$CodeDialog.hide()


func _on_block_pressed():
	if await GameData.blockUser(self.userID):
		$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/Control/MarginContainer/Block.visible = false


func _on_graphics_push_on_toggled(toggled_on):
	GameData.localSettings["graphics"] = "advanced"
	GameData.saveLocalSettings()


func _on_graphics_push_off_toggled(toggled_on):
	GameData.localSettings["graphics"] = "simple"
	GameData.saveLocalSettings()


func _on_customize_profile_pressed() -> void:
	GameData.gotoCustomizeProfile()

func _on_test_game_pressed():
	GameData.viewGame(-1, false)


func _on_match_history_pressed():
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.show()
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings.hide()


func _on_leaderboard_pressed():
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.hide()
	$MarginContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings.show()
