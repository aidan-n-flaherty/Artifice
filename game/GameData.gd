extends Node

signal gamesChanged

signal userChanged

signal chatChanged(chatID)

var Game = preload("res://Game.tscn")

var GameDetail = preload("res://GameDetail.tscn")

var currentTab = null

var current_scene = null

var openGameIDs = {}

var ongoingGameIDs = {}

var pastGameIDs = {}

var gameDetails = {}

var games = {}

var chats = {}

var chatGroups = {}

var id: int

var token: int

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
	id = 3
	token = 5577006791947779410
	
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

func loadChat(chatID: int):
	var chat = await HTTPManager.getReq("/fetchChat", {
		"chatID": chatID
	})
	
	if chat and hasGame(chat.gameID):
		self.chats[chatID] = chat
		self.chatGroups[chat.gameID].push_front(chat)
		
		emit_signal("chatChanged", chatID)

func loadChats(gameID: int):
	var chatGroup = await HTTPManager.getReq("/fetchChats", {
		"gameID": gameID
	})
	
	if(chatGroup):
		self.chatGroups[gameID] = chatGroup
		
		for chat in chatGroup:
			self.chats[chat.id] = chat
		
		return chatGroup
	
	return []
	
func loadGames():
	var openGames = await HTTPManager.getReq("/fetchGames")
	
	for game in openGames: 
		openGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game
	
	var ongoingGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": false
	})
	
	for game in ongoingGames:
		ongoingGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game
	
	var pastGames = await HTTPManager.getReq("/fetchUserGames", {
		"past": true
	})
	
	for game in pastGames:
		pastGameIDs[int(game.gameData.id)] = true
		gameDetails[int(game.gameData.id)] = game

func joinGame(id: int, password = ""):
	var response = await HTTPManager.postReq("/joinMatch", {}, {
		"gameID": id,
		"password": password
	})
	
	if not response: return
	
	var game = await HTTPManager.getReq("/fetchGameDetails", {
		"gameID": id
	}, false)
	
	if not game: return
	
	addGame(game)

func addGame(game):
	ongoingGameIDs[int(game.gameData.id)] = true
	gameDetails[int(game.gameData.id)] = game
	
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

func loadGameState(id: int):
	var gameData = await HTTPManager.getReq("/fetchGameState", {
		"gameID": id
	})
	
	games[id] = GameInterface.new()
	games[id].init(id, self.id, int(Time.get_unix_time_from_system() + 5), gameData.users, getGameDetails(id).gameSettings.settingOverrides)
	
	bulkAddOrders(games[id], gameData.orders)

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

func addMessage(message):
	chats[message.chatID].messages.push_back(message)
	
	emit_signal("chatChanged", message.chatID)

func getSelf():
	return user
	
func getID():
	return user.id
	
func getUser(id: int):
	return users[id]

func getChats(gameID: int):
	return chatGroups[gameID]
	
func getChat(chatID: int):
	return chats[chatID]

func getGameDetails(id: int):
	return gameDetails[id]
	
func hasGame(id: int):
	return games.has(id)
	
func hasChat(id: int):
	return chats.has(id)
	
func chatInGame(chatID: int, gameID: int):
	return getChats(gameID).has(getChat(chatID))

func getGame(id: int):
	return games[id]

func getOpenGames():
	return openGameIDs.keys()
	
func getOngoingGames():
	return ongoingGameIDs.keys()

func getPastGames():
	return pastGameIDs.keys()
