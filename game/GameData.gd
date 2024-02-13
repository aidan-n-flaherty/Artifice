extends Node

signal gamesChanged

signal gameChanged(gameID)

signal userChanged

signal chatChanged(chatID)

signal queuesChanged

var Game = preload("res://Game.tscn")

var GameDetail = preload("res://GameDetail.tscn")

var currentTab = null

var current_scene = null

var queues = {}

var openGameIDs = {}

var ongoingGameIDs = {}

var pastGameIDs = {}

var gameDetails = {}

var games = {}

var messageBuffer = {}

var chats = {}

var chatGroups = {}

var id: int

var token: String

var user

var users = {}

func _ready():
	var root = get_tree().get_root()
	current_scene = root.get_child(root.get_child_count() - 1)
	
	login()
	loadSelf()
	loadGames()
	
	WebSocketManager.init(token)

func goto_scene(path):
	call_deferred("_deferred_goto_scene", path)

func _deferred_goto_scene(path):
	current_scene.queue_free()

	var s = ResourceLoader.load(path)
	current_scene = s.instantiate()

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)

func goto_node(node) -> void:
	call_deferred("_deferred_goto_node", node)
	
func _deferred_goto_node(node) -> void:
	current_scene.queue_free()
	
	current_scene = node

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)

func login():
	#id = 3
	#token = "5577006791947779410"
	#id = 4
	#token = "8674665223082153551"
	id = 5
	token = "15352856648520921629"
	
func viewGame(id: int):
	if not hasGame(id):
		await loadGameState(id)
	
	var node = Game.instantiate()
	node.init(id)

	goto_node(node)
	
func viewGameDetails(id: int):
	var node = GameDetail.instantiate()
	node.gameID = id

	goto_node(node)

func loadSelf():
	var user = await HTTPManager.getReq("/fetchSelf")
	
	if not user: return
	
	self.user = user
	
func loadUser(userID: int):
	var user = await HTTPManager.getReq("/fetchUser", {
		"id": userID
	})
	
	if not user: return null
	
	users[userID] = user
	
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
		self.chatGroups[int(chat.gameID)].push_front(chat)
		
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
		self.chatGroups[gameID] = chatGroup
		
		for chat in chatGroup:
			self.chats[int(chat.id)] = chat
		
		return chatGroup
	
	return []

func loadQueues():
	var statuses = await HTTPManager.getReq("/fetchQueues")
	
	queues = {}
	
	if statuses:
		for status in statuses:
			queues[status.queueName] = status
	
	return queues

func loadOpenGames():
	var openGames = await HTTPManager.getReq("/fetchGames")
	
	for game in openGames: 
		openGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadOngoingGames():
	var ongoingGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": false
	})
	
	for game in ongoingGames:
		ongoingGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadPastGames():
	var pastGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": true
	})
	
	for game in pastGames:
		pastGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func loadGames():
	loadOpenGames()
	
	loadOngoingGames()
	
	loadPastGames()

func joinQueue(queueType: String):
	if await HTTPManager.postReq("/joinQueue", {}, {
		"queueType": queueType,
	}):
		emit_signal("queuesChanged")

func leaveQueue(queueType: String):
	if await HTTPManager.postReq("/leaveQueue", {}, {
		"queueType": queueType,
	}):
		emit_signal("queuesChanged")

func joinGame(id: int, password = ""):
	var response = await HTTPManager.postReq("/joinMatch", {}, {
		"gameID": id,
		"password": password
	})
	
	if not response: return false
	
	var game = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if not game: return false
	
	addGame(game)
	
	return true

func addGame(game):
	ongoingGameIDs[int(game.gameData.id)] = true
	gameDetails[int(game.gameData.id)] = game
	
	if openGameIDs.has(int(game.gameData.id)):
		openGameIDs.erase(int(game.gameData.id))
	
	emit_signal("gamesChanged")

func bulkAddOrders(game, orders):
	for order in orders:
		game.bulkAddOrder(order.type, int(order.id), int(order.referenceID), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	game.endBulkAdd()

func updateOrders(id: int):
	if not hasGame(id):
		return
	
	var orderData = await HTTPManager.getReq("/fetchOrders", {
		"gameID": id,
		"referenceID": games[id].getReferenceID()
	})
	
	bulkAddOrders(games[id], orderData)

func getGameUsers(id: int):
	return await HTTPManager.getReq("/fetchGameUsers", {
		"gameID": id
	})

func loadGameUsers(id: int):
	var users = await getGameUsers(id)
	
	if not users:
		return
	
	var details = getGameDetails(id)
	
	if games.has(id):
		games[id].init(id, self.id, details.gameData.startTime, details.gameSettings.playerCap, users, details.gameSettings.settingOverrides)

func loadGameSettings(id: int):
	var users = await getGameUsers(id)
	
	if not users:
		return
	
	var details = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	})
	
	if not details:
		return
	
	gameDetails[id] = details
	
	if games.has(id):
		games[id].init(id, self.id, details.gameData.startTime, details.gameSettings.playerCap, users, details.gameSettings.settingOverrides)
	
func loadGameState(id: int):
	var gameState = await HTTPManager.getReq("/fetchGameState", {
		"gameID": id
	})
	
	var details = getGameDetails(id)
	
	games[id] = GameInterface.new()
	games[id].init(id, self.id, details.gameData.startTime, details.gameSettings.playerCap, gameState.users, details.gameSettings.settingOverrides)
	
	bulkAddOrders(games[id], gameState.orders)

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
	
	game.addOrder(order.type, int(order.id), int(order.referenceID), float(order.timestamp), int(order.senderID), PackedInt32Array(order.argumentIDs), int(order.argumentIDs.size()))
	
	print("Order registered")

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

func verifyEnd(gameID: int):
	if await HTTPManager.getReq("/verifyGameEnd", {
		"gameID": gameID
	}):
		gameDetails[gameID].gameData.finished = true
		if openGameIDs.has(gameID):
			openGameIDs.erase(gameID)
		if pastGameIDs.has(gameID):
			pastGameIDs[gameID] = true
		return true
	
	return false
	
func viewEnd(gameID: int):
	WebSocketManager.sendMessage("[VIEWGAMEEND]" + str(gameID))
	emit_signal("gamesChanged")
	
func isFinished(gameID: int):
	return gameDetails[id].gameData.finished

func getSelf():
	return user
	
func getID():
	return user.id
	
func getUser(id: int):
	return users[id] if users.has(id) else await loadUser(id)

func getChats(gameID: int):
	return chatGroups[gameID]
	
func getChat(chatID: int):
	return chats[chatID]

func getQueue(queueType: String):
	return queues[queueType]

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

func getPastGames():
	return pastGameIDs.keys()
