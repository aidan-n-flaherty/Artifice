extends Node
var GameList = preload("res://GameList.tscn")
var GameButton = preload("res://GameButton.tscn")


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func createGameList(gameIDs):
	var list = GameList.instantiate()
	list.gameIDs = gameIDs
	
	return list
