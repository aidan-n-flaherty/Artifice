extends Node

var Game = preload("res://Game.tscn")

var GameDetail = preload("res://GameDetail.tscn")

var current_scene = null

var openGameIDs = {}

var ongoingGameIDs = {}

var pastGameIDs = {}

var gameDetails = {}

var games = {}

var id

var token

func _ready():
	var root = get_tree().get_root()
	current_scene = root.get_child(root.get_child_count() - 1)
	
	login()
	loadGames()

func goto_scene(path):
	call_deferred("_deferred_goto_scene", path)

func _deferred_goto_scene(path):
	# It is now safe to remove the current scene
	current_scene.queue_free()

	# Load the new scene.
	var s = ResourceLoader.load(path)

	# Instance the new scene.
	current_scene = s.instantiate()

	# Add it to the active scene, as child of root.
	get_tree().get_root().add_child(current_scene)

	# Optionally, to make it compatible with the SceneTree.change_scene() API.
	get_tree().set_current_scene(current_scene)


func goto_node(node) -> void:
	call_deferred("_deferred_goto_node", node)
	
func _deferred_goto_node(node) -> void:
	current_scene.queue_free()
	
	current_scene = node

	get_tree().get_root().add_child(current_scene)
	get_tree().set_current_scene(current_scene)

func viewGame(id):
	if(!games.has(id)):
		games[id] = GameInterface.new()
		games[id].init(self.id)
	
	var node = Game.instantiate()
	node.gameID = id

	goto_node(node)
	
	
func viewGameDetails(id):
	var node = GameDetail.instantiate()
	node.gameID = id

	goto_node(node)
	
func login():
	id = 654321
	token = 5577006791947779410
	
func loadGames():
	var openGames = await HTTPManager.getReq("/fetchGames", {
		"token": token
	})
	
	for game in openGames:
		openGameIDs[game.gameData.id] = true
		gameDetails[game.gameData.id] = game
	
	var ongoingGames = await HTTPManager.getReq("/fetchUserGames", {
		"token": token,
		"past": false
	})
	
	for game in ongoingGames:
		ongoingGameIDs[game.gameData.id] = true
		gameDetails[game.gameData.id] = game
	
	var pastGames = await HTTPManager.getReq("/fetchUserGames", {
		"token": token,
		"past": true
	})
	
	for game in pastGames:
		pastGameIDs[game.gameData.id] = true
		gameDetails[game.gameData.id] = game

func addGame(game):
	ongoingGameIDs[game.gameData.id] = true
	gameDetails[game.gameData.id] = game
	
func loadGame(id):
	var gameData = await HTTPManager.getReq("fetchGameState", {
		"tokens": token,
		"gameID": id
	})
	
	games[id] = GameInterface.new()
	
func getGameDetails(id):
	return gameDetails[id]

func getGame(id):
	return games[id]

func getOpenGames():
	return openGameIDs.keys()
	
func getOngoingGames():
	return ongoingGameIDs.keys()

func getPastGames():
	return pastGameIDs.keys()
