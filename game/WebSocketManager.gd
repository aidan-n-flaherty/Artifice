extends Node

# The URL we will connect to
var websocket_url = "wss://chemiserver.com/temp/ws"

var socket: WebSocketPeer

var expBackoff: int

var timer: float

func _ready():
	pass

func init(token: String):
	socket = WebSocketPeer.new()
	var err = socket.connect_to_url(websocket_url + "?token=" + str(token))
	
	if not err:
		expBackoff = 1
		timer = 0.0

func sendMessage(message: String):
	var state = socket.get_ready_state()
	
	if state == WebSocketPeer.STATE_OPEN:
		socket.send_text(message)
	else:
		print("Could not send message, socket is closed")

func processMessage(message: String):
	var messageObj = JSON.parse_string(message)
	
	if messageObj:
		GameData.addMessage(messageObj)
	elif message.begins_with("[GAMEUPDATE]"):
		message.erase(0, len("[GAMEUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.updateOrders(gameID)
	elif message.begins_with("[GAMEUSERSUPDATE]"):
		message.erase(0, len("[GAMEUSERSUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadGameUsers(gameID)
	elif message.begins_with("[GAMESETTINGSUPDATE]"):
		message.erase(0, len("[GAMESETTINGSUPDATE]"))
		
		var gameID = message.to_int()
		
		GameData.loadGameSettings(gameID)
	elif message.begins_with("[CHAT]"):
		message.erase(0, len("[CHAT]"))
		
		var chatID = message.to_int()
		
		GameData.loadChat(chatID)

func _process(delta):
	if not socket:
		timer += delta
		
		if timer > expBackoff:
			timer = 0
			expBackoff *= 2
			
			init(GameData.token)
		
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
