package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"
	"time"
)

func version(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/version" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	response := map[string]string{
		"version": latestVersion,
	}

	returnData, err := json.Marshal(response); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

func getWSConn(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Received new Client connection")

	ws, err := wsUpgrader.Upgrade(w, r, nil)
	if err != nil {
		fmt.Println(err)
		return
	}

	id := uint32(0)

	if c, ok := connections[id]; ok {
		fmt.Println("Replaced duplicate")
		c.Close()
	}
	connections[id] = ws

	processConnection(ws, id, 60*time.Second)

	fmt.Println("Client upgrade to WS success")
}

// POST, Arguments: user (User)
// signs a user up using the user data provided, gives the user their id (necessary for login)
// and a temporary auth token
func signup(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/signup" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	var user User
	err := json.NewDecoder(r.Body).Decode(&user)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	str := "INSERT INTO profiles (username, password, pushEnabled) VALUES(?, ?, ?);"

	dbPass, err := hashPassword(user.Password)
	if err != nil {
		fmt.Println(err)
	}

	res, err := db.Exec(str, user.Username, dbPass, user.PushEnabled)
	if err != nil {
		fmt.Println(err)
		return
	}

	id, err := res.LastInsertId()
	if err != nil {
		fmt.Println(err)
		return
	}

	uniqueKey, err := getToken(db, uint32(id), user.Password)
	if err != nil {
		fmt.Println(err)
		return
	}

	response := map[string]string{
		"token": strconv.FormatUint(uint64(uniqueKey), 10),
		"id":    strconv.FormatUint(uint64(id), 10), // if the client doesn't save this, they won't be able to log in again
	}

	returnData, err := json.Marshal(response); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusCreated)
	w.Write(returnData)
}

// GET, Arguments: id (int), password (string), pushToken (string)
// logs a user in using an id and client auto-generated password, returns a temporary auth token
func login(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/login" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	var data map[string]string
	err := json.NewDecoder(r.Body).Decode(&data)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	_, hasID := data["id"]
	_, hasPassword := data["password"]
	_, hasPushToken := data["pushToken"]
	if !hasID || !hasPassword {
		http.Error(w, "Missing arguments.", http.StatusBadRequest)
		return
	}

	id, err := strconv.ParseUint(data["id"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	uniqueKey, err := getToken(db, uint32(id), data["password"])
	if err != nil {
		http.Error(w, err.Error(), http.StatusUnauthorized)
		return
	}

	token := strconv.FormatUint(uint64(uniqueKey), 10)

	if hasPushToken && data["pushToken"] != "" {
		addPushToken(db, token, data["pushToken"])
	}

	response := map[string]string{"token": token}

	returnData, err := json.Marshal(response); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token
// returns all games currently available to join
func fetchGames(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchGames" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	response, err := getGames(db, data["token"])
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	returnData, err := json.Marshal(response); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string), gameID (int)
// get the data of a specific game
func fetchGameDetails(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchGameDetails" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "id")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	gameID, err := strconv.ParseUint(data["gameID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	response, err := getGame(db, data["token"], uint32(gameID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string), past (boolean)
// returns all games that a user has access to
// returns ongoing or past games based on the 'past' boolean parameter
func fetchUserGames(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchUserGames" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "past")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	response, err := userGames(db, data["token"], data["past"] == "true")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string), gameID (int)
// gets the game state (players and orders) of a game the user is a player in
func fetchGameState(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchGameState" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "gameID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	gameID, err := strconv.ParseUint(data["gameID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	response, err := getGameState(db, data["token"], uint32(gameID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// POST, Arguments: token (string), settings (GameSettings)
// creates a new game where the user is the host
func createMatch(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/createMatch" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	var settings GameSettings
	err = json.NewDecoder(r.Body).Decode(&settings)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	game, err := createGame(db, data["token"], settings)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(game)

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusCreated)
	w.Write(returnData)
}

// PUT, Arguments: token (string), game (GameDetails)
// edits a game if the user is the host
func editMatch(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/editMatch" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "PUT" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	var game GameDetails
	err = json.NewDecoder(r.Body).Decode(&game)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	if err = editGame(db, data["token"], game); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// POST, Arguments: token (string), gameID (int), password (string)
// joins a match, must supply a password (if the room doesn't have a password,
// the password can be anything
func joinMatch(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/joinMatch" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "gameID", "password")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	gameID, err := strconv.ParseUint(data["gameID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	if !joinGame(db, data["token"], uint32(gameID), data["password"]) {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// PATCH, Arguments: token (string), gameID (int)
// leaves a match that the user is a player in
func leaveMatch(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/leaveMatch" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "PATCH" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "gameID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	gameID, err := strconv.ParseUint(data["gameID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	if !leaveGame(db, data["token"], uint32(gameID)) {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// PUT, Arguments: token (string), gameID (int)
// creates a new order if the order doesn't already exist, otherwise modifies
// an existing order, sends back the new order in both cases
func updateOrder(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/updateOrder" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "PUT" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "gameID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	var order Order
	err = json.NewDecoder(r.Body).Decode(&order)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	order, err = uploadOrder(db, data["token"], order)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	returnData, err := json.Marshal(order); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string)
// fetches the user data associated with a login token
func fetchSelf(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchSelf" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	response, err := getSelf(db, data["token"])
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: id (string)
// fetches user data for a specific user by ID
func fetchUser(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchUser" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "id")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	userID, err := strconv.ParseUint(data["id"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	response, err := getUser(db, uint32(userID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(response); if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// PUT, Arguments: token (string), user (User)
// takes in a User object and modifies the user data
func editSelf(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/editSelf" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "PUT" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	var user User
	err = json.NewDecoder(r.Body).Decode(&user)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	err = editUser(db, data["token"], user)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// GET, Arguments: token (string), chatID (int)
// loads a specific chat from its ID
func fetchChat(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchChat" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "chatID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	chatID, err := strconv.ParseUint(data["chatID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	response, err := getChat(db, data["token"], uint32(chatID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string), gameID (int)
// gets the IDs of all chats within a game to be fetched individually when necessary
func fetchChats(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchChats" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "gameID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	gameID, err := strconv.ParseUint(data["gameID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	response, err := getChats(db, data["token"], uint32(gameID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// GET, Arguments: token (string), chatID (int), offset (int)
// gets the next 20 messages in a chat, assuming that the client has every most recent message
// reaching back until the offset
func fetchMessages(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/fetchMessages" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "GET" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "chatID", "offset")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	chatID, err := strconv.ParseUint(data["chatID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	offset, err := strconv.Atoi(data["offset"])
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	response, err := getMessages(db, data["token"], uint32(chatID), offset)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	returnData, err := json.Marshal(response)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(http.StatusAccepted)
	w.Write(returnData)
}

// POST, Arguments: token (string), messageID (int)
// reports a message for moderation, won't actually do anything for awhile
func reportMessage(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/reportMessage" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "messageID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	messageID, err := strconv.ParseUint(data["messageID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	err = flagMessage(db, data["token"], uint32(messageID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// POST, Arguments: token (string), userID (int)
// blocks a specific user so no new messages will be received from them
func block(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/block" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "userID")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	userID, err := strconv.ParseUint(data["userID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	err = blockUser(db, data["token"], uint32(userID))
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

// POST, Arguments: token (string), chatID (int), content (string)
// sends a text message to a chat, automatically broadcasts to all listeners
func sendMessage(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/sendMessage" {
		http.Error(w, "404 not found.", http.StatusNotFound)
		fmt.Printf("Invalid path\n")
		return
	}

	if r.Method != "POST" {
		http.Error(w, "Method is not supported.", http.StatusNotFound)
		fmt.Printf("Method not supported\n")
		return
	}

	data, err := getValues(r, "token", "chatID", "content")
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	chatID, err := strconv.ParseUint(data["chatID"], 10, 32)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	err = sendTextMessage(db, data["token"], uint32(chatID), data["content"])
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
	}

	w.WriteHeader(http.StatusAccepted)
}
