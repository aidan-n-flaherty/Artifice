extends MarginContainer

signal deselected(conversation)

var chatID

var chat

var participants

var messages = {}

var gameID

var game

var temporary

var playerTags = []

@onready var container = $MarginContainer/VBoxContainer/Control/ScrollContainer 

var atBottom = true

var atTop = true

var prevSize = 0

var prevTextEditSize = 0

var maintainScroll = false

var received = false

var sending = false

var maxWidth = 0

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)
	scroll_to_bottom()
	$MarginContainer/VBoxContainer/Control/ScrollContainer.get_v_scroll_bar().connect("value_changed", scroll_changed)
	
	await get_tree().process_frame
	self.maxWidth = $MarginContainer/VBoxContainer/Control.size.x
	
	if chat:
		refresh(chat.messages)
	
func scroll_to_bottom():
	container.set_deferred("scroll_vertical",container.get_v_scroll_bar().max_value)


func initTemp(gameID):
	temporary = true
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for player in game.getPlayers():
		if player.getUserID() == GameData.getID():
			continue
		
		var playerTag = preload("res://PlayerTag.tscn").instantiate()
		
		playerTag.init(player, false, true)
		playerTags.push_back(playerTag)
		$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/PlayerList.add_child(playerTag)

func init(gameID: int, chatID: int):
	temporary = false
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for tag in playerTags:
		$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/PlayerList.remove_child(tag)
		tag.queue_free()
	playerTags.clear()
	
	chat = GameData.getChat(chatID)
	self.chatID = chatID
	
	for player in game.getPlayers():
		if PackedInt32Array(chat.participants).has(player.getUserID()):
			var playerTag = preload("res://PlayerTag.tscn").instantiate()
		
			playerTag.init(player, true, false)
			playerTags.push_back(playerTag)
			$MarginContainer/VBoxContainer/HBoxContainer/ScrollContainer/PlayerList.add_child(playerTag)
	
func refresh(messageList):
	messageList = messageList.filter(func(message): return message.chatID == self.chatID)

	messageList.sort_custom(func(a, b): return a.timestamp < b.timestamp)
	
	var lastSenderID = -1
	var lastTimestamp = -1
	
	var players = {}
	for player in game.getPlayers():
		players[player.getUserID()] = player
	
	for id in messages.keys():
		var messageExists = false
		
		for message in messageList:
			if message.id == id:
				messageExists = true
				break
		
		if not messageExists:
			messages.erase(id)
	
	for index in len(messageList):
		var message = messageList[index]
		
		var messageNode
		
		
		if messages.has(message.id):
			messageNode = messages[message.id]
		else:
			messageNode = preload("res://Message.tscn").instantiate()
			if players.has(int(message.senderID)):
				messageNode.init(maxWidth, message, players[int(message.senderID)].getColor())
			else:
				messageNode.init(maxWidth, message, Color.DIM_GRAY)
			
			$MarginContainer/VBoxContainer/Control/ScrollContainer/MessageContainer.add_child(messageNode)
			
		messageNode.displayName(lastSenderID != message.senderID or message.timestamp > lastTimestamp + 10 * 60)
		messageNode.displayTime(message.timestamp > lastTimestamp + 10 * 60)
		
		if lastSenderID != message.senderID:
			lastSenderID = message.senderID
		
		lastTimestamp = message.timestamp
		
		messages[message.id] = messageNode
			
		$MarginContainer/VBoxContainer/Control/ScrollContainer/MessageContainer.move_child(messageNode, index)
	
	GameData.readChat(chatID)
	
	var anyUnread = false
	for chat in GameData.getChats(gameID):
		if len(chat.messages) > 0 and chat.messages[len(chat.messages) - 1].timestamp > chat.readTimestamp:
			anyUnread = true
			break
			
	if not anyUnread:
		var details = GameData.getGameDetails(gameID)
		details.gameData.hasChatNotifications = false

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if self.maxWidth != $MarginContainer/VBoxContainer/Control.size.x:
		self.maxWidth = $MarginContainer/VBoxContainer/Control.size.x
		
		for messageNode in self.messages.values():
			messageNode.setMaxWidth(self.maxWidth)
	
	var canSend = false
	for tag in playerTags:
		if tag.isSelected():
			canSend = true
			break
	
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = not canSend or sending
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.modulate = Color(0.9, 0.9, 0.9) if (not canSend or sending) else Color(1.0, 1.0, 1.0)

func chatChanged(chatID: int):
	if chatID == self.chatID:
		if chat:
			refresh(chat.messages)
		else:
			init(gameID, self.chatID)

func _on_back_pressed():
	emit_signal("deselected", self)

func _on_send_pressed():
	received = true
	print("send pressed")
	if $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text == "":
		return
	
	sending = true
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = true
	
	print("disabled ", $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled)
	if !chatID:
		var playerIDs = PackedInt32Array()
		for tag in playerTags:
			if tag.isSelected():
				playerIDs.push_back(tag.getUserID())
		
		var tmp = await GameData.createChat(gameID, playerIDs)

		if tmp:
			chatID = int(tmp)
		else:
			sending = false
			return
	
	var content = $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text
	if await GameData.sendMessage(chatID, content):
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.clear()
	
	sending = false

func scroll_changed(value):
	atBottom = container.scroll_vertical == container.get_v_scroll_bar().max_value - container.size.y
	
	if atBottom:
		maintainScroll = false
	
	if not atTop and container.scroll_vertical < container.get_v_scroll_bar().min_value + 40:
		atTop = true
		prevSize = $MarginContainer/VBoxContainer/Control/ScrollContainer/MessageContainer.size.y
		maintainScroll = true
		GameData.loadMessages(chatID)
		
	if container.scroll_vertical >= container.get_v_scroll_bar().min_value + 80:
		atTop = false

func _on_scroll_container_scroll_ended():
	#see the new message at the bottom only if the bar is already at the bottom
	print("scroll_vertical is ",container.scroll_vertical,", Y-size is ",container.size.y)

func _on_message_container_resized():
	if atBottom:
		call_deferred("scroll_to_bottom")
	elif maintainScroll:
		container.set_deferred("scroll_vertical", $MarginContainer/VBoxContainer/Control/ScrollContainer/MessageContainer.size.y - prevSize)
		
		container.swipe_start = Vector2(container.get_h_scroll(), $MarginContainer/VBoxContainer/Control/ScrollContainer/MessageContainer.size.y - prevSize)
		container.swipe_mouse_start = get_global_mouse_position()
		container.swipe_mouse_times = [Time.get_ticks_msec()]
		container.swipe_mouse_positions = [container.swipe_mouse_start]


func _on_text_edit_text_changed(new_text: String):
	if len($MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text) > 255:
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit["theme_override_colors/font_color"] = Color.FIREBRICK
	else:
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit["theme_override_colors/font_color"] = Color.WHITE
	
	return
	var lines = 0
	
	for i in range(0, $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.get_line_count()):
		lines += 1 + $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.get_line_wrap_count(i)
	
	if lines < 5:
		prevTextEditSize = $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.size.y
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.custom_minimum_size.y = 0
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.scroll_fit_content_height = true
	elif prevTextEditSize > 0:
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.custom_minimum_size.y = prevTextEditSize
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.scroll_fit_content_height = false


func _on_resized():
	call_deferred("scroll_to_bottom")


func _on_text_edit_resized():
	if container and atBottom:
		call_deferred("scroll_to_bottom")
