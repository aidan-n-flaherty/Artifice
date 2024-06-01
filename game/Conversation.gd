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

@onready var container = $MarginContainer/VBoxContainer/ScrollContainer 

var atBottom = true

var atTop = true

var prevSize = 0

var prevTextEditSize = 0

var maintainScroll = false

var received = false

# Called when the node enters the scene tree for the first time.
func _ready():
	GameData.chatChanged.connect(chatChanged)
	container.set_deferred("scroll_vertical",9999999)
	$MarginContainer/VBoxContainer/ScrollContainer.get_v_scroll_bar().connect("value_changed", scroll_changed)
	
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
		$MarginContainer/VBoxContainer/HBoxContainer/PlayerList.add_child(playerTag)

func init(gameID: int, chatID: int):
	temporary = false
	
	game = GameData.getGame(gameID)
	self.gameID = gameID
	
	for tag in playerTags:
		$MarginContainer/VBoxContainer/HBoxContainer/PlayerList.remove_child(tag)
		tag.queue_free()
	playerTags.clear()
	
	chat = GameData.getChat(chatID)
	self.chatID = chatID
	
	for player in game.getPlayers():
		if PackedInt32Array(chat.participants).has(player.getUserID()):
			var playerTag = preload("res://PlayerTag.tscn").instantiate()
		
			playerTag.init(player, true, false)
			playerTags.push_back(playerTag)
			$MarginContainer/VBoxContainer/HBoxContainer/PlayerList.add_child(playerTag)
	
	refresh(chat.messages)
	
func refresh(messageList):
	messageList.sort_custom(func(a, b): return a.timestamp < b.timestamp)
	
	var lastSenderID = -1
	var lastTimestamp = -1
	
	var players = {}
	for player in game.getPlayers():
		players[player.getUserID()] = player
	
	for index in len(messageList):
		var message = messageList[index]
		
		var messageNode
		
		
		if messages.has(message.id):
			messageNode = messages[message.id]
		else:
			messageNode = preload("res://Message.tscn").instantiate()
			messageNode.init(message, players[int(message.senderID)].getColor())
			$MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.add_child(messageNode)
			
		messageNode.displayName(lastSenderID != message.senderID or message.timestamp > lastTimestamp + 10 * 60)
		messageNode.displayTime(message.timestamp > lastTimestamp + 10 * 60)
		
		if lastSenderID != message.senderID:
			lastSenderID = message.senderID
		
		lastTimestamp = message.timestamp
		
		messages[message.id] = messageNode
			
		$MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.move_child(messageNode, index)
	
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
	var canSend = false
	for tag in playerTags:
		if tag.isSelected():
			canSend = true
			break
	
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = not canSend

func chatChanged(chatID: int):
	if chat and chatID == chat.id:
		refresh(chat.messages)
	else:
		init(gameID, chatID)

func _on_back_pressed():
	emit_signal("deselected", self)

func _on_send_pressed():
	received = true
	print("send pressed")
	if $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text == "":
		return
	
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = true
	
	if !chatID:
		var playerIDs = PackedInt32Array()
		for tag in playerTags:
			if tag.isSelected():
				playerIDs.push_back(tag.getUserID())
		
		var tmp = await GameData.createChat(gameID, playerIDs)

		if tmp:
			chatID = int(tmp)
		else:
			$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = false
			return
	
	if await GameData.sendMessage(chatID, $MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.text):
		$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/TextEdit.clear()
	
	$MarginContainer/VBoxContainer/MarginContainer/MarginContainer/HBoxContainer/Send.disabled = false

func scroll_changed(value):
	atBottom = container.scroll_vertical == container.get_v_scroll_bar().max_value - container.size.y
	
	if not atTop and container.scroll_vertical < container.get_v_scroll_bar().min_value + 40:
		atTop = true
		prevSize = $MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.size.y
		maintainScroll = true
		GameData.loadMessages(chatID)
		
	if container.scroll_vertical >= container.get_v_scroll_bar().min_value + 80:
		atTop = false

func _on_scroll_container_scroll_ended():
	#see the new message at the bottom only if the bar is already at the bottom
	print("scroll_vertical is ",container.scroll_vertical,", Y-size is ",container.size.y)


func _on_message_container_resized():
	if atBottom:
		print("Debug: scrolling to the bottom")
		call_deferred("scroll_to_bottom")
	elif maintainScroll:
		maintainScroll = false
		container.set_deferred("scroll_vertical", container.scroll_vertical + $MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.size.y - prevSize)
		
		container.swipe_start = Vector2(container.get_h_scroll(), container.scroll_vertical + $MarginContainer/VBoxContainer/ScrollContainer/MessageContainer.size.y - prevSize)
		container.swipe_mouse_start = get_global_mouse_position()
		container.swipe_mouse_times = [Time.get_ticks_msec()]
		container.swipe_mouse_positions = [container.swipe_mouse_start]


func _on_text_edit_text_changed():
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
