extends Node

signal gamesChanged

signal gameChanged(gameID)

signal userChanged(userID)

signal chatChanged(chatID)

signal queuesChanged

signal menuSwitched(menu)

signal menuFade

signal loadUserDetail(userID)

signal loadGame(gameID, past)

signal loadGameDetail(gameID)

var version = "1.5"

var needsUpdate = false

var currentTab = null

var current_scene = null

var queues = {}

var openGameIDs = {}

var ongoingGameIDs = {}

var gameDetails = {}

var games = {}

var gameUsers = {}

var messageBuffer = {}

var chats = {}

var chatGroups = {}

var currentGameIDs = []

var id: int

var password: String

var token: String

var user

var users = {}

var pastUserGameIDs = {}

var baseResolution = Vector2i(1080, 1920)

var mutex = Mutex.new()

var thread

var byteBrew = null

var pushToken = ""

var pushTokenSet = false

var iCloudEnabled = true

var localSettings = {}

"""func _apn_device(value):
	pushToken = value
	print("Push token: ", pushToken)
	
	mutex.lock()
	if token:
		pushTokenSet = true
		
		var returnVal = await HTTPManager.postReq("/updatePushToken", {}, {
			"pushToken": pushToken
		})
		
		print("Push token update: ", returnVal)
	mutex.unlock()"""
	
func _ready():
	current_scene = get_tree().current_scene
	#if Engine.has_singleton("APN"):
	#	var _apn = Engine.get_singleton("APN");
	#	_apn.connect("device_address_changed", _apn_device);
	#	_apn.register_push_notifications(_apn.PUSH_SOUND | _apn.PUSH_BADGE | _apn.PUSH_ALERT);
	
	
	if Engine.has_singleton("ByteBrew"):
		byteBrew = Engine.get_singleton("ByteBrew")
		if OS.get_name() == "Android":
			byteBrew.InitializeByteBrew("scIQdIELG", "qmTQjInepgltlGwoUfjx879xOLUSPbQ4r0NDxFFYObYo/ya7jpreFl5eJ91vfORd", Engine.get_version_info().string, "1.0")
		if OS.get_name() == "iOS":
			byteBrew.InitializeByteBrew("eKK1bQDm1", "9LQjPNIHxho60LGa5IleSrpbB3iSmBE+Zr+mWcNW2oBETMQjc3xIj0l94S5kmAO+", Engine.get_version_info().string, "1.0")
			byteBrew.StartPushNotifications()
	
	get_viewport().connect("size_changed", resize)
	resize()
	
	var response = await HTTPManager.getReq("/version", {}, false)
	
	var versionArr = response["version"].split(".") if response and response["version"] else "1.0"
	var currentVersionArr = GameData.version.split(".")
	
	var outdated = false
	
	while len(versionArr) < len(currentVersionArr):
		versionArr.append("0")
		
	while len(currentVersionArr) < len(versionArr):
		currentVersionArr.append("0")
	
	for i in range(len(currentVersionArr)):
		if versionArr[i].to_int() > currentVersionArr[i].to_int():
			outdated = true
			break
		elif versionArr[i].to_int() < currentVersionArr[i].to_int():
			break

	loadLocalSettings()

	if outdated:
		needsUpdate = true
		goto_scene("res://OutdatedVersion.tscn")
	else:
		await login()
	
	#mutex.lock()
	#if not pushTokenSet:
	#	var returnVal = await HTTPManager.postReq("/updatePushToken", {}, {
	#		"pushToken": pushToken
	#	})
	#mutex.unlock()
	

func resize():
	get_tree().get_root().content_scale_factor = max(max(1.0, min(1.25, (baseResolution.x * 1.0 / baseResolution.y) / (get_viewport().size.x * 1.0 / get_viewport().size.y))), min(2.0, get_viewport().size.x * 1.0 / get_viewport().size.y))

func goto_login():
	call_deferred("_deferred_goto_login")

func _deferred_goto_login():
	if current_scene:
		get_tree().get_root().remove_child(current_scene)
		current_scene.queue_free()

	var s = ResourceLoader.load("res://MainMenu.tscn")
	current_scene = s.instantiate()

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)
	
	await login()
	
func goto_scene(path):
	call_deferred("_deferred_goto_scene", path)

func _deferred_goto_scene(path):
	if current_scene:
		get_tree().get_root().remove_child(current_scene)
		current_scene.queue_free()

	var s = ResourceLoader.load(path)
	current_scene = s.instantiate()

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)

func goto_node(node) -> void:
	call_deferred("_deferred_goto_node", node)
	
func _deferred_goto_node(node) -> void:
	if current_scene:
		get_tree().get_root().remove_child(current_scene)
		current_scene.queue_free()
	
	current_scene = node

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)

func loadLocalSettings():
	var file = FileAccess.open("user://artifice_settings.save", FileAccess.READ)
		
	if file:
		localSettings = JSON.parse_string(file.get_as_text())
	
	return localSettings

func saveLocalSettings():
	var file = FileAccess.open("user://artifice_settings.save", FileAccess.WRITE)
	file.store_string(JSON.stringify(localSettings))

func storeAuth(authObj):
	if iCloudEnabled and OS.get_name() == "iOS" and Engine.has_singleton("ICloud"):
		var iCloud = Engine.get_singleton("ICloud")
		
		if iCloud.synchronize_key_values() == OK:
			iCloud.set_key_values({
				"auth": authObj
			})
		else:
			print("Could not synchronize iCloud values")
	
	var file = FileAccess.open("user://artifice_data.save", FileAccess.WRITE)
	file.store_string(JSON.stringify(authObj))

func getAuth():
	var auth
	
	if iCloudEnabled and OS.get_name() == "iOS" and Engine.has_singleton("ICloud"):
		var iCloud = Engine.get_singleton("ICloud")
		
		if iCloud.synchronize_key_values() == OK:
			auth = iCloud.get_key_value("auth")
			print("Got authentication from iCloud")
		else:
			print("Could not synchronize iCloud values")
	
	if not auth:
		var file = FileAccess.open("user://artifice_data.save", FileAccess.READ)
		
		if file:
			auth = JSON.parse_string(file.get_as_text())
	
	return auth

func startShare():
	return await HTTPManager.getReq("/startShare", {
		"id": id,
		"password": password
	}, false)
	
func getShare(code: String):
	var auth = await HTTPManager.getReq("/getShare", {
		"code": code
	}, false)
	
	if auth:
		id = auth.id
		password = auth.password
		
		var authObj = {
			"id": id,
			"password": password
		}
		
		storeAuth(authObj)
		
		goto_login()
		
		return true
	
	return false

func requestCode(phoneNumber: String):
	return await HTTPManager.getReq("/verifyPhone", {
		"phoneNumber": phoneNumber
	}, false)

func signup():
	print("Signing up...")
	var username = "unnamed"
	password = ""
	
	var characters = []
	for i in range(0, 10):
		characters.append(str(i))
	
	for i in range(0, 26):
		characters.append(str(char(97 + i)))
	
	for i in range(0, 64):
		password += characters[randi() % len(characters)]
	
	var response = await HTTPManager.postReq("/signup", {
		"username": username,
		"password": password
	}, {
		#"phoneNumber": phoneNumber,
		#"code": code
	}, false)
	
	print(response)
	
	if not response: return false
	
	id = int(response.id)
	
	var authObj = {
		"id": id,
		"password": password
	}
	
	storeAuth(authObj)
	
	print("Signed up!")
	
	goto_login()

func deleteAccount():
	print("Deleting account...")
	
	var response = await HTTPManager.putReq("/deleteAccount", {}, {})
	
	print(response)
	
	if not response: return false
	
	if iCloudEnabled and OS.get_name() == "iOS" and Engine.has_singleton("ICloud"):
		var iCloud = Engine.get_singleton("ICloud")
		
		iCloud.remove_key("auth")
	
	var file = FileAccess.open("user://artifice_data.save", FileAccess.WRITE)
		
	goto_scene("res://EULA.tscn")

func login():
	print("Logging in...")
	
	var auth = getAuth()

	if auth and int(auth.id) != 0 and str(auth.password) != "":
		id = int(auth.id)
		password = str(auth.password)
		
		var expBackoff = 2
		
		var pushToken = byteBrew.GetUserID() if byteBrew else ""
		print("Token ", pushToken)
		
		while true:
			var response = await HTTPManager.postReq("/login", {
				"id": id,
				"password": password,
				"pushToken": pushToken
			}, {}, false)
		
			if response:
				token = response.token
				break
			
			await get_tree().create_timer(expBackoff).timeout
			expBackoff *= 2
		
		loadSelf()
		loadGames()
		
		gameUsers.clear()
		for arr in currentGameIDs:
			loadGameUsers(arr[0])
			updateOrders(arr[0])
			loadChats(arr[0])
		
		for id in chats:
			refreshMessages(id)
		
		if not WebSocketManager.hasSocket():
			WebSocketManager.init(token)
		
		print("Logged in!")
		print("ID:", id)
		print("Token", token)
	else:
		goto_scene("res://EULA.tscn")
	
	#id = 3
	#token = "5577006791947779410"
	#id = 4
	#token = "8674665223082153551"
	#id = 5
	#token = "15352856648520921629"

func getCurrentGameID():
	return currentGameIDs[len(currentGameIDs) - 1][0]

func changeGame(id: int):
	emit_signal("gameChanged", id)

func viewUser(id: int):
	emit_signal("loadUserDetail", id)

func viewUserCompletion(id: int):
	var node = preload("res://UserDetails.tscn").instantiate()
	node.init(id)
		
	goto_node(node)

func viewGameDetail(id: int):
	emit_signal("loadGameDetail", id)

func viewGameDetailCompletion(id: int):
	var details = getGameDetails(id)
	
	if len(currentGameIDs) == 0 or currentGameIDs[len(currentGameIDs) - 1][0] != id:
		currentGameIDs.push_back([id, false])
	
	if int(details.gameData.hostID) == getSelfID():
		var gameChanger = preload("res://GameChanger.tscn").instantiate()
		gameChanger.init(id)
		
		goto_node(gameChanger)
	else:
		var gameDetail = preload("res://GameDetail.tscn").instantiate()
		gameDetail.init(id)
		
		goto_node(gameDetail)

func viewGame(id: int, past=false):
	emit_signal("loadGame", id, past)

func viewGameCompletion(id: int, past=false):
	if not hasGame(id):
		await loadGameState(id)
	
	if past:
		games[id].startAtEnd()
	
	gameDetails[id].gameData.hasNotifications = false
	
	games[id].set_process(false)
	games[id].set_visible(false)
	
	var node = preload("res://Game.tscn").instantiate()
	node.init(id)

	if len(currentGameIDs) == 0 or currentGameIDs[len(currentGameIDs) - 1][0] != id:	
		currentGameIDs.push_back([id, past])
	
	goto_node(node)

func exitGameToMenu():
	if len(currentGameIDs) > 0:
		currentGameIDs.remove_at(len(currentGameIDs) - 1)
	
	if len(currentGameIDs) > 0:
		if hasGame(currentGameIDs[len(currentGameIDs) - 1][0]):
			viewGame(currentGameIDs[len(currentGameIDs) - 1][0], currentGameIDs[len(currentGameIDs) - 1][1])
		else:
			viewGameDetail(currentGameIDs[len(currentGameIDs) - 1][0])
		return false
	else:
		return true
	
func viewGameDetails(id: int):
	var node = preload("res://GameDetail.tscn").instantiate()
	node.gameID = id

	if len(currentGameIDs) == 0 or currentGameIDs[len(currentGameIDs) - 1][0] != id:
		currentGameIDs.push_back([id, false])
	
	goto_node(node)

func getSelfID():
	return id

func loadSelf():
	var user = await HTTPManager.getReq("/fetchSelf")
	
	if not user: return
	
	self.user = user
	
	emit_signal("userChanged", id)
	
func editSelf(user):
	return await HTTPManager.putReq("/editSelf", user, {})
	
func loadUser(userID: int):
	print("Loading user " + str(userID))
	var user = await HTTPManager.getReq("/fetchUser", {
		"id": userID
	})
	
	if not user: return null
	
	users[userID] = user
	
	emit_signal("userChanged", userID)
	
	return user
	
func createChat(gameID: int, users: PackedInt32Array):
	var returnVal = await HTTPManager.postReq("/makeChat", users, {
		"gameID": gameID
	})
	
	if not returnVal:
		return false
	
	return returnVal["chatID"]

func loadChat(chatID: int):
	var chat = await HTTPManager.getReq("/fetchChat", {
		"chatID": chatID
	})
	
	if chat and hasGame(chat.gameID):
		self.chats[chatID] = chat
		if not self.chatGroups.has(int(chat.gameID)):
			self.chatGroups[int(chat.gameID)] = [chatID]
		elif not self.chatGroups[int(chat.gameID)].has(chatID):
			self.chatGroups[int(chat.gameID)].push_front(chatID)
		
		if messageBuffer.has(chat.id):
			var messages = messageBuffer[chat.id].filter(func(m1): return not chat.messages.any(func(m2): return m1.id == m2.id))
			chat.messages.append_array(messages)
			messageBuffer.erase(chat.id)
		
		emit_signal("chatChanged", chatID)

func loadChats(gameID: int):
	var chatGroup = await HTTPManager.getReq("/fetchChats", {
		"gameID": gameID
	})
	
	if(chatGroup):
		self.chatGroups[gameID] = []
		
		for chat in chatGroup:
			self.chats[int(chat.id)] = chat
			self.chatGroups[gameID].push_back(int(chat.id))
	else:
		self.chatGroups[gameID] = []
	
	return getChats(gameID)

func refreshMessages(chatID: int):
	var chat = getChat(chatID)
	
	if chat:
		chat.messages = []
		
		await loadMessages(chatID)

func loadMessages(chatID: int):
	var chat = getChat(chatID)
	
	if chat:
		var messages = await HTTPManager.getReq("/fetchMessages", {
			"chatID": chatID,
			"offset": len(chat.messages)
		})
		
		if messages:
			messages = messages.filter(func(m1): return not chat.messages.any(func(m2): return m1.id == m2.id))
			chat.messages.append_array(messages)
			
			emit_signal("chatChanged", chatID)

func loadQueues():
	var statuses = await HTTPManager.getReq("/fetchQueues")
	
	self.queues = {}
	
	if statuses:
		for status in statuses:
			self.queues[status.queueName] = status
	
	emit_signal("queuesChanged")

func loadOpenGames():
	var openGames = await HTTPManager.getReq("/fetchGames")
	
	if not openGames:
		return
	
	for game in openGames: 
		openGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadOpenGame(gameID: int):
	var game = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if not game:
		return
	
	openGameIDs[int(game.gameData.id)] = true
	gameDetails[int(game.gameData.id)] = game
	
	emit_signal("gamesChanged")

func loadOngoingGames():
	var ongoingGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": false
	})
	
	if not ongoingGames:
		return
	
	for game in ongoingGames:
		ongoingGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadPastGames():
	var pastGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": true
	})
	
	if not pastGames:
		return
	
	if not pastUserGameIDs.has(getSelfID()):
		pastUserGameIDs[getSelfID()] = {}
	
	for game in pastGames:
		pastUserGameIDs[getSelfID()][int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadPastUserGames(userID: int):
	var pastGames = await HTTPManager.getReq("/fetchPastGames", {
		"userID": userID
	})
	
	if not pastGames:
		return {}
	
	if not pastUserGameIDs.has(userID):
		pastUserGameIDs[userID] = {}
	
	for game in pastGames:
		pastUserGameIDs[userID][int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game
	
	return pastUserGameIDs[userID]

func loadGames():
	await loadOpenGames()
	
	await loadOngoingGames()
	
	await loadPastGames()
	
	emit_signal("gamesChanged")

func joinQueue(queueType: String):
	await HTTPManager.postReq("/joinQueue", {}, {
		"queueType": queueType,
	})
	
	await loadQueues()

func leaveQueue(queueType: String):
	await HTTPManager.postReq("/leaveQueue", {}, {
		"queueType": queueType,
	})
	
	await loadQueues()

func joinGame(id: int, gamePassword = ""):
	var response = await HTTPManager.postReq("/joinMatch", {}, {
		"gameID": id,
		"password": gamePassword
	})
	
	if not response: return false
	
	var game = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if not game: return false
	
	addGame(game)
	
	await GameData.loadGameUsers(id)
	
	return true

func leaveGame(id: int):
	var response = await HTTPManager.postReq("/leaveMatch", {}, {
		"gameID": id
	})
	
	if response:
		if ongoingGameIDs.has(int(id)):
			ongoingGameIDs.erase(int(id))
		return true
	
	return false

func openQuickMatch(id: int, password = ""):
	var game = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if game:
		addGame(game)
		viewGame(id)

func addGame(game):
	ongoingGameIDs[int(game.gameData.id)] = true
	gameDetails[int(game.gameData.id)] = game
	
	if openGameIDs.has(int(game.gameData.id)):
		openGameIDs.erase(int(game.gameData.id))
	
	emit_signal("gamesChanged")

func updateOrders(id: int):
	if not hasGame(id):
		return
	
	var orderData = await HTTPManager.getReq("/fetchOrders", {
		"gameID": id,
		"referenceID": games[id].getReferenceID()
	})
	
	bulkAddOrders(id, games[id], orderData)
	
	if len(orderData) > 0:
		emit_signal("gameChanged", id)

func getGameUsers(id: int):
	if not gameUsers.has(id):
		await loadGameUsers(id)
		
	return gameUsers[id]

func loadGameUsers(id: int):
	print("Loading users")
	var users = await HTTPManager.getReq("/fetchGameUsers", {
		"gameID": id
	})
	print("Users ", users)
	
	if not users:
		gameUsers[id] = {}
		return
	
	gameUsers[id] = users
	
	if gameDetails.has(id):
		gameDetails[id].gameData.playerCount = len(users)
	
	emit_signal("gameChanged", id)
	
	emit_signal("gamesChanged")

func loadGameSettings(id: int):
	var details = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if not details:
		return
	
	gameDetails[id] = details
	
	if details.gameData.started and gameUsers.has(id) and len(gameUsers[id].keys()) < details.gameSettings.playerCap:
		await loadGameUsers(id)
	
	if games.has(id):
		var game = games[id]
		var users = await getGameUsers(id)
		
		game.init(id, self.id, details.gameData.seed, details.gameData.startTime, details.gameData.finished, details.gameSettings.playerCap, users, details.gameSettings.settingOverrides)
	
	emit_signal("gameChanged", id)
	
	emit_signal("gamesChanged")

func loadGameState(id: int):
	var gameState = await HTTPManager.getReq("/fetchGameState", {
		"gameID": id
	})
	
	if not gameState:
		return
	
	var details = getGameDetails(id)
	
	var game = GameInterface.new()
	game.init(id, self.id, details.gameData.seed, details.gameData.startTime, details.gameData.finished, details.gameSettings.playerCap, gameState.users, details.gameSettings.settingOverrides)
	game.set_visible(false)
	game.set_process(false)
	
	bulkAddOrders(id, game, gameState.orders)
	
	mutex.lock()
	games[id] = game
	mutex.unlock()

func bulkAddOrders(gameID: int, game, orders):
	for order in orders:
		game.bulkAddOrder(order.type, int(order.id), int(order.referenceID), bool(order.canceled), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	game.endBulkAdd()
	
	if len(orders) > 0 and gameDetails.has(gameID) and not gameID in currentGameIDs:
		gameDetails[gameID].gameData.hasNotifications = true

func addOrder(gameID: int, type, referenceID, timestamp, arguments):
	var game = getGame(gameID)
	
	print(game.getTime())
	var order = await HTTPManager.putReq("/updateOrder", {
		"type": type,
		"referenceID": referenceID,
		"timestamp": timestamp,
		"argumentIDs": arguments
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), bool(order.canceled), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")

func replaceOrder(orderID: int, gameID: int, type, referenceID, timestamp, arguments):
	var game = getGame(gameID)
	
	print(game.getTime())
	var order = await HTTPManager.putReq("/updateOrder", {
		"id": orderID,
		"type": type,
		"referenceID": referenceID,
		"timestamp": timestamp,
		"argumentIDs": arguments
	}, {
		"gameID": gameID
	})
	
	print(order)

	if(!order): return;
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), bool(order.canceled), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order re-registered")

func cancelOrder(gameID: int, orderID: int):
	var game = getGame(gameID)
	
	if game and orderID != -1:
		var response = await HTTPManager.postReq("/removeOrder", {}, {
			"gameID": gameID,
			"orderID": orderID
		})
	
		print(response)
		
		if !response:
			return
	
		game.cancelOrder(orderID)
		
		print("Canceled ", orderID)

func sendMessage(chatID: int, content: String) -> bool:
	return await HTTPManager.postReq("/sendMessage", {
		"chatID": chatID,
		"content": content
	}, {})

func addMessage(message):
	if not chats.has(int(message.chatID)):
		if messageBuffer.has(message.chatID):
			messageBuffer[message.chatID].push_back(message)
		else:
			messageBuffer[message.chatID] = [ message ]
		
		return
	
	for m in chats[int(message.chatID)].messages:
		if message.id == m.id:
			return
	
	chats[int(message.chatID)].messages.push_back(message)
	
	emit_signal("chatChanged", int(message.chatID))

func readChat(chatID: int):
	WebSocketManager.sendMessage("[READCHAT]" + str(chatID))
	
	if chats.has(chatID):
		chats[chatID].readTimestamp = Time.get_unix_time_from_system()

func reportUser(userID: int, reason: String) -> bool:
	return await HTTPManager.postReq("/reportUser", {}, {
		"userID": userID,
		"reason": reason
	})
	
func blockUser(userID: int) -> bool:
	if await HTTPManager.postReq("/block", {}, {
		"userID": userID
	}):
		for chat in chats.values():
			var newMessages = []
			
			for message in chat.messages:
				if int(message.senderID) != userID:
					newMessages.append(message)
			
			if len(newMessages) < len(chat.messages):
				chat.messages = newMessages
				emit_signal("chatChanged", chat.id)
		
		return true
	return false

func verifyEnd(gameID: int):
	if await HTTPManager.getReq("/verifyGameEnd", {
		"gameID": gameID
	}):
		gameDetails[gameID].gameData.finished = true
		if ongoingGameIDs.has(gameID):
			ongoingGameIDs.erase(gameID)
		
		if not pastUserGameIDs.has(getSelfID()):
			pastUserGameIDs[getSelfID()] = {}
		
		pastUserGameIDs[getSelfID()][gameID] = true
		return true
	
	return false
	
func viewEnd(gameID: int):
	WebSocketManager.sendMessage("[VIEWGAMEEND]" + str(gameID))
	
	if ongoingGameIDs.has(gameID):
		ongoingGameIDs.erase(gameID)
	if not pastUserGameIDs.has(gameID):
		pastUserGameIDs[gameID] = true
	
	emit_signal("gamesChanged")

func isFinished(gameID: int):
	return gameDetails[gameID].gameData.finished

func getSelf():
	return user
	
func getID():
	return user.id
	
func getUser(id: int):
	return users[id] if users.has(id) else await loadUser(id)

func getChats(gameID: int):
	var chatIDs = chatGroups[gameID]
	var chats = []
	
	for id in chatIDs:
		chats.push_back(getChat(id))
	
	return chats

func getChat(chatID: int):
	return chats[chatID]

func getQueue(queueType: String):
	return queues[queueType] if queues.has(queueType) else null

func getQueues():
	return queues

func getGameDetails(id: int):
	return gameDetails[id]
	
func hasGame(id: int):
	return games.has(id)
	
func hasChat(id: int):
	return chats.has(id)
	
func chatInGame(chatID: int, gameID: int):
	return getChat(chatID).gameID == gameID

func getGame(id: int):
	return games[id]

func getOpenGames():
	return openGameIDs.keys()
	
func getOngoingGames():
	return ongoingGameIDs.keys()

func getPastGames(userID: int):
	return pastUserGameIDs[userID].keys() if pastUserGameIDs.has(userID) else (await loadPastUserGames(userID)).keys()

func _exit_tree():
	for game in games.values():
		game.queue_free()
	
