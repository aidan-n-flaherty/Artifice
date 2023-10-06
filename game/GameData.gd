extends Node

signal gamesChanged

signal userChanged

var Game = preload("res://Game.tscn")

var GameDetail = preload("res://GameDetail.tscn")

var currentTab = null

var current_scene = null

var openGameIDs = {}

var ongoingGameIDs = {}

var pastGameIDs = {}

var gameDetails = {}

var games = {}

var id: int

var token: int

var user

func _ready():
	var root = get_tree().get_root()
	current_scene = root.get_child(root.get_child_count() - 1)
	
	login()
	loadUser()
	loadGames()

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
	id = 654321
	token = 5577006791947779410
	
func viewGame(id: int):
	#if(!games.has(id)):
	games[id] = GameInterface.new()
	games[id].init(id, self.id, getGameDetails(id).gameSettings.settingOverrides)
	
	var node = Game.instantiate()
	node.init(id)

	goto_node(node)
	
func viewGameDetails(id: int):
	var node = GameDetail.instantiate()
	node.gameID = id

	goto_node(node)

func loadUser():
	var user = await HTTPManager.getReq("/fetchSelf")
	
	if not user: return
	
	self.user = user
	
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
	
func loadGameState(id: int):
	var gameData = await HTTPManager.getReq("fetchGameState", {
		"tokens": token,
		"gameID": id
	})
	
	games[id] = GameInterface.new()

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
	
func getUser():
	return user

func getGameDetails(id: int):
	return gameDetails[id]

func getGame(id: int):
	return games[id]

func getOpenGames():
	return openGameIDs.keys()
	
func getOngoingGames():
	return ongoingGameIDs.keys()

func getPastGames():
	return pastGameIDs.keys()
