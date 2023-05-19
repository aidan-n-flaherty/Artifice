extends Node

var url = "https://chemiserver.com/temp"

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func postReq(path, data, includeToken=true):
	var json = JSON.stringify(data)
	var headers = ["Content-Type: application/json"]
	
	var reqNode = HTTPRequest.new()
	add_child(reqNode)
	
	if(includeToken):
		path += str("?token=", GameData.token)
		
	reqNode.request(url + path, headers, HTTPClient.METHOD_POST, json)
	var response = await reqNode.request_completed
	
	var result = response[0]
	var response_code = response[1]
	var response_headers = response[2]

	var body = JSON.parse_string(response[3].get_string_from_utf8())
	
	remove_child(reqNode)
	
	if(response_code < 200 || response_code > 299):
		print("Bad response code")
		
	if(body == null):
		print("Bad JSON")
	
	return body

func putReq(path, data, params, includeToken=true):
	var json = JSON.stringify(data)
	var headers = ["Content-Type: application/json"]
	
	var reqNode = HTTPRequest.new()
	add_child(reqNode)
	
	var first = true
	if(includeToken):
		path += str("?token=", GameData.token)
		first = false
		
	for key in params.keys():
		if first:
			path += "?"
			first = false
		else:
			path += "&"
		path += key + "=" + str(params[key])
		
	reqNode.request(url + path, headers, HTTPClient.METHOD_PUT, json)
	var response = await reqNode.request_completed

	var result = response[0]
	var response_code = response[1]
	var response_headers = response[2]
	var body = JSON.parse_string(response[3].get_string_from_utf8())
	
	remove_child(reqNode)
	
	if(response_code < 200 || response_code > 299):
		print("Bad response code")
		
	if(body == null):
		print("Bad JSON")
	
	return body
	
func getReq(path, params):
	var reqNode = HTTPRequest.new()
	add_child(reqNode)
	
	var first = true
	for key in params.keys():
		if first:
			path += "?"
			first = false
		else:
			path += "&"
		path += key + "=" + str(params[key])
	
	reqNode.request(url + path, [], HTTPClient.METHOD_GET)
	var response = await reqNode.request_completed
	
	var result = response[0]
	var response_code = response[1]
	var response_headers = response[2]
	var body = JSON.parse_string(response[3].get_string_from_utf8())
	
	remove_child(reqNode)
	
	if(response_code < 200 || response_code > 299):
		print("Bad response code")
		
	if(body == null):
		print("Bad JSON")
	
	return body

	
