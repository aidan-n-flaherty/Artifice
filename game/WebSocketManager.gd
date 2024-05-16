extends Node

# The URL we will connect to
var websocket_url = "wss://artificeserver.com/ws"

var socket: WebSocketPeer

var expBackoff = 1

var timer: float

var start = false

func _ready():
	pass

func hasSocket():
	return socket != null

func init(token: String):
	socket = WebSocketPeer.new()
	var err = socket.connect_to_url(websocket_url + "?token=" + str(token))
	
	if not err:
		expBackoff = 1
		timer = 0.0
	
	start = true

func sendMessage(message: String):
	var state = socket.get_ready_state()
	
	if state == WebSocketPeer.STATE_OPEN:
		socket.send_text(message)
	else:
		print("Could not send message, socket is closed")

func processMessage(message: String):
	var messageObj = JSON.parse_string(message)
	
	print(messageObj)
	if messageObj:
		GameData.addMessage(messageObj)
	elif message.begins_with("[GAMEUPDATE]"):
		message.erase(0, len("[GAMEUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.updateOrders(gameID)
	elif message.begins_with("[GAMECREATE]"):
		message.erase(0, len("[GAMECREATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadOpenGame(gameID)
	elif message.begins_with("[GAMEUSERSUPDATE]"):
		message.erase(0, len("[GAMEUSERSUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadGameUsers(gameID)
	elif message.begins_with("[GAMESETTINGSUPDATE]"):
		message.erase(0, len("[GAMESETTINGSUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadGameSettings(gameID)
	elif message.begins_with("[GAMEUSERSSETTINGSUPDATE]"):
		message.erase(0, len("[GAMEUSERSSETTINGSUPDATE]"))
		
		var gameID = message.to_int()
		
		await GameData.loadGameUsers(gameID)
		GameData.loadGameSettings(gameID)
	elif message.begins_with("[QUEUEMATCH]"):
		message.erase(0, len("[QUEUEMATCH]"))
		
		var gameID = message.to_int()
		
		GameData.openQuickMatch(gameID)
	elif message.begins_with("[QUEUEUPDATE]"):
		message.erase(0, len("[QUEUEUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadQueues()
	elif message.begins_with("[CHAT]"):
		message.erase(0, len("[CHAT]"))
		
		var chatID = message.to_int()
		
		GameData.loadChat(chatID)

func _process(delta):
	if not start:
		return
	
	if not socket:
		timer += delta
		
		if timer > expBackoff:
			timer = 0
			expBackoff *= 2
			
			init(GameData.token)
			
			if socket:
				var response = await HTTPManager.getReq("/version", {}, false)
	
				if response and response["version"] != GameData.version:
					GameData.needsUpdate = true
					GameData.goto_scene("res://OutdatedVersion.tscn")
				else:
					await GameData.login()
		
		return
	
	socket.poll()
	var state = socket.get_ready_state()
	
	if state == WebSocketPeer.STATE_OPEN:
		while socket.get_available_packet_count():
			processMessage(socket.get_packet().get_string_from_utf8())
	elif state == WebSocketPeer.STATE_CLOSING:
		# Keep polling to achieve proper close.
		pass
	elif state == WebSocketPeer.STATE_CLOSED:
		var code = socket.get_close_code()
		var reason = socket.get_close_reason()
		print("WebSocket closed with code: %d, reason %s. Clean: %s" % [code, reason, code != -1])
		socket = null
		
		timer = 0.0
		expBackoff = 1
