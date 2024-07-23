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
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList/MarginContainer.add_theme_constant_override("margin_right", 0)
	
	if isSelf:
		user = GameData.getSelf()
		
		updateUser(userID)
		
		await GameData.loadSelf()
	else:
		$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.suppressReady()
		$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.initUser(userID)
		await $MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.init()
		await GameData.loadUser(userID)

func updateUser(userID):
	if userID != self.userID:
		return
	
	if isSelf:
		user = GameData.getSelf()
	else:
		user = await GameData.getUser(self.userID)
	
	if(!user): return
	
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Restore.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Report.visible = not isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Block.visible = not isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText.editable = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText.text = user.username
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/VictoryContainer/MarginContainer/HBoxContainer/Victories.text = str(user.userStats.gamesWon)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/DefeatsContainer/MarginContainer/HBoxContainer/Defeats.text = str(user.userStats.gamesLost)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/TotalGamesContainer/MarginContainer/HBoxContainer/TotalGames.text = str(user.userStats.gamesPlayed)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/AverageRankContainer/MarginContainer/HBoxContainer/AverageRank.text = "N/A" if user.userStats.gamesPlayed == 0 else "%d%%" % int(user.userStats.averageRank * 100.0 / user.userStats.gamesPlayed)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/RatingContainer/MarginContainer/HBoxContainer/Rating.text = str(user.userStats.rating)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/PlacementContainer/MarginContainer/HBoxContainer/Placement.text = "#" + str(user.userStats.placement + 1)
	
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Gold/MarginContainer/HBoxContainer/Gold.text = str(user.userStats.gold)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Silver/MarginContainer/HBoxContainer/Silver.text = str(user.userStats.silver)
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Bronze/MarginContainer/HBoxContainer/Bronze.text = str(user.userStats.bronze)
	
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/PushNotifications.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/PushNotificationButtons.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/PushNotificationButtons/PushOn.set_pressed_no_signal(user.pushEnabled)
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/PushNotificationButtons/PushOff.set_pressed_no_signal(not user.pushEnabled)
	
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/ChatPushNotifications.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons/PushOn.set_pressed_no_signal(user.chatPushEnabled)
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/ChatPushNotificationButtons/PushOff.set_pressed_no_signal(not user.chatPushEnabled)
	
	var settings = GameData.loadLocalSettings()
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/Graphics.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/GraphicsButtons.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/GraphicsButtons/PushOn.set_pressed_no_signal(not settings.has("graphics") or settings["graphics"] == "advanced")
	$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/GraphicsButtons/PushOff.set_pressed_no_signal(settings.has("graphics") and settings["graphics"] == "simple")
	
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Delete.visible = isSelf
	
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Leaderboard.visible = isSelf
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/HBoxContainer/Spacer.visible = isSelf
	
	if isSelf:
		var users = await GameData.getRankings()
		
		for child in $MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings/VBoxContainer.get_children():
			child.queue_free()
		
		var i = 1
		for player in users:
			var node = preload("res://RankingView.tscn").instantiate()
			node.init(player.id, player.username, player.userStats.rating, i)
			$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings/VBoxContainer.add_child(node)
			
			i += 1


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if update > 0:
		update -= delta
		
		if update <= 0:
			GameData.editSelf(user)
	

func _on_username_text_text_changed(new_text):
	if len(new_text) < 3 or len(new_text) > 20:
		$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.FIREBRICK
		return
	else:
		$MarginContainer/ScrollContainer/VBoxContainer/GridContainer/UsernameText["theme_override_colors/font_color"] = Color.WHITE
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
		$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Report.visible = false


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
		$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/Control/MarginContainer/Block.visible = false


func _on_graphics_push_on_toggled(toggled_on):
	GameData.localSettings["graphics"] = "advanced"
	GameData.saveLocalSettings()


func _on_graphics_push_off_toggled(toggled_on):
	GameData.localSettings["graphics"] = "simple"
	GameData.saveLocalSettings()

func _on_match_history_pressed():
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.show()
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings.hide()


func _on_leaderboard_pressed():
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/PastGameList.hide()
	$MarginContainer/ScrollContainer/VBoxContainer/HBoxContainer/VBoxContainer/ScrollContainer/UserRankings.show()
