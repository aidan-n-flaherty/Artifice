package main

import (
	"database/sql"
	"errors"
	"fmt"
	"strconv"
	"strings"
	"time"

	"github.com/gorilla/websocket"
)

// Get the details of all games that currently are accepting players
func getGames(db *sql.DB, token string) ([]GameDetails, error) {
	var response = make([]GameDetails, 0)

	userID, err := getID(db, token)
	if err != nil {
		return response, err
	}

	query := "SELECT id, hostID, lobbyName, password, playerCount, playerCap, ratingConstraints, activeTimes, createdAt, startTime, simulationSpeed, version FROM games WHERE startTime < NOW() AND playerCount < playerCap;"

	results, err := db.Query(query)
	if err != nil {
		return response, err
	}

	defer results.Close()

	for results.Next() {
		var game GameDetails

		err = results.Scan(&game.Data.HostID, &game.Settings.LobbyName, &game.Settings.Password,
			&game.Data.PlayerCount, &game.Settings.PlayerCap, &game.Settings.RatingConstraints,
			&game.Settings.ActiveTimes, &game.Data.CreatedAt, &game.Settings.StartTime,
			&game.Settings.SimulationSpeed, &game.Data.Version)
		if err != nil {
			continue
		}

		if game.Settings.Password != "" {
			game.Data.HasPassword = true
		}

		if game.Data.HostID != userID && game.Data.HasPassword {
			game.Settings.Password = ""
		}

		response = append(response, game)
	}

	return response, nil
}

// Get the details of a specific game
func getGame(db *sql.DB, token string, gameID uint32) (GameDetails, error) {
	var game GameDetails

	userID, err := getID(db, token)
	if err != nil {
		return game, err
	}

	query := "SELECT id, hostID, lobbyName, password, playerCount, playerCap, ratingConstraints, activeTimes, weekdays, createdAt, startTime, simulationSpeed, version FROM games WHERE id = ?;"

	results, err := db.Query(query, gameID)
	if err != nil {
		return game, err
	}

	defer results.Close()

	if !results.Next() {
		return game, errors.New("No game found.")
	}

	var ratingConstraints string
	var activeTimes string
	var weekdays string
	err = results.Scan(&game.Data.ID, &game.Data.HostID, &game.Settings.LobbyName, &game.Settings.Password,
		&game.Data.PlayerCount, &game.Settings.PlayerCap, &ratingConstraints,
		&activeTimes, &weekdays, &game.Data.CreatedAt, &game.Settings.StartTime,
		&game.Settings.SimulationSpeed, &game.Data.Version)

	game.Settings.RatingConstraints = deserialize(ratingConstraints)
	game.Settings.ActiveTimes = strings.Split(activeTimes, ",")
	game.Settings.Weekdays = strings.Split(weekdays, ",")

	if err != nil {
		return game, err
	}

	if game.Settings.Password != "" {
		game.Data.HasPassword = true
	}

	if game.Data.HostID != userID && game.Data.HasPassword {
		game.Settings.Password = ""
	}

	return game, nil
}

// Get the details of all games that the user is participating in
// 'history' argument dictates whether ongoing or concluded games are returned
func userGames(db *sql.DB, token string, history bool) ([]GameDetails, error) {
	var response = make([]GameDetails, 0)

	userID, err := getID(db, token)
	if err != nil {
		return response, err
	}

	query := "SELECT id, hostID, lobbyName, password, playerCount, playerCap, ratingConstraints, activeTimes, weekdays, UNIX_TIMESTAMP(createdAt), UNIX_TIMESTAMP(startTime), simulationSpeed, version FROM games INNER JOIN participants ON participantID = ? AND gameID = id;"

	results, err := db.Query(query, userID)
	if err != nil {
		return response, err
	}

	defer results.Close()

	for results.Next() {
		var game GameDetails

		var ratingConstraints string
		var activeTimes string
		var weekdays string
		err = results.Scan(&game.Data.ID, &game.Data.HostID, &game.Settings.LobbyName,
			&game.Settings.Password, &game.Data.PlayerCount, &game.Settings.PlayerCap,
			&ratingConstraints, &activeTimes, &weekdays, &game.Data.CreatedAt,
			&game.Settings.StartTime, &game.Settings.SimulationSpeed, &game.Data.Version)
		if err != nil {
			continue
		}

		game.Settings.RatingConstraints = deserialize(ratingConstraints)
		game.Settings.ActiveTimes = strings.Split(activeTimes, ",")
		game.Settings.Weekdays = strings.Split(weekdays, ",")

		if game.Settings.Password != "" {
			game.Data.HasPassword = true
		}

		if game.Data.HostID != userID && game.Data.HasPassword {
			game.Settings.Password = ""
		}

		response = append(response, game)
	}

	return response, nil
}

// Acquire the full game state necessary to simulate a game.
func getGameState(db *sql.DB, token string, gameID uint32) (GameState, error) {
	var response GameState

	userID, err := getID(db, token)
	if err != nil {
		return response, err
	}

	query := "SELECT id, username, gamesWon, gamesLost, averageRank, rating, tier, participantNumber FROM participants INNER JOIN users ON id = participantID WHERE gameID = ?;"

	results, err := db.Query(query, gameID)
	if err != nil {
		return response, err
	}

	defer results.Close()

	for results.Next() {
		var user User
		var participantNumber int

		err = results.Scan(&user.ID, &user.Username, &user.Stats.GamesWon, &user.Stats.GamesLost, &user.Stats.AverageRank, &user.Stats.Rating, &participantNumber)
		if err != nil {
			return response, err
		}

		response.Players[participantNumber] = user
	}

	// select all orders that have already been sent or were sent by this user
	query = "SELECT id, senderID, type, parameters, UNIX_TIMESTAMP(timestamp) FROM orders CROSS JOIN sentOrders WHERE gameID = ? AND (timestamp <= NOW() OR senderID = ?);"

	results, err = db.Query(query, gameID, userID)
	if err != nil {
		return response, err
	}

	defer results.Close()

	for results.Next() {
		var order Order

		err = results.Scan(&order.ID, &order.SenderID, &order.Type, &order.ArgumentIDs, &order.Timestamp)
		if err != nil {
			return response, err
		}

		response.Orders = append(response.Orders, order)
	}

	return response, nil
}

// create a new game, client needs to fetch all current games to access it
func createGame(db *sql.DB, token string, settings GameSettings) (GameDetails, error) {
	var game GameDetails

	hostID, err := getID(db, token)
	if err != nil {
		return game, err
	}

	query := "INSERT INTO games (hostID, lobbyName, password, playerCap, ratingConstraints, weekdays, activeTimes, simulationSpeed, version) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"

	result, err := db.Exec(query, hostID, settings.LobbyName, settings.Password, settings.PlayerCap,
		serialize(settings.RatingConstraints), strings.Join(settings.Weekdays, ","),
		strings.Join(settings.ActiveTimes, ","), settings.SimulationSpeed, latestVersion)
	if err != nil {
		return game, err
	}

	gameID, err := result.LastInsertId()
	if err != nil {
		return game, err
	}

	joinGame(db, token, uint32(gameID), settings.Password)

	return getGame(db, token, uint32(gameID))
}

// modifies all arguments passed in the Settings field
// can only edit a game before it starts
func editGame(db *sql.DB, token string, game GameDetails) error {
	hostID, err := getID(db, token)
	if err != nil {
		return err
	}

	query := "UPDATE games SET lobbyName = ?, password = ?, playerCap = ?, ratingConstraints = ?, weekdays = ?, activeTimes = ?, simulationSpeed = ? WHERE id = ? AND hostID = ? AND startTime > NOW();"

	_, err = db.Exec(query, game.Settings.LobbyName, game.Settings.Password, game.Settings.PlayerCap,
		serialize(game.Settings.RatingConstraints), strings.Join(game.Settings.Weekdays, ","),
		strings.Join(game.Settings.ActiveTimes, ","), game.Settings.SimulationSpeed, game.Data.ID, hostID)
	if err != nil {
		return err
	}

	return nil
}

// deletes a game, clients should not have direct access to this method
func deleteGame(db *sql.DB, gameID uint32) error {
	_, err := db.Exec("DELETE FROM games WHERE gameID = ?;", gameID)
	if err != nil {
		return err
	}

	_, err = db.Exec("DELETE FROM chats WHERE gameID = ?;", gameID)
	if err != nil {
		return err
	}

	return nil
}

// returns a boolean representing whether the user could join the game
func joinGame(db *sql.DB, token string, gameID uint32, passwordAttempt string) bool {
	userID, err := getID(db, token)
	if err != nil {
		return false
	}

	query := "SELECT password, playerCount, playerCap, NOW() >= startTime FROM games WHERE id = ?;"

	results, err := db.Query(query, gameID)
	if err != nil {
		return false
	}

	defer results.Close()

	if !results.Next() {
		return false
	}

	var password string
	var playerCount int
	var playerCap int
	var alreadyStarted bool

	err = results.Scan(&password, &playerCount, &playerCap, &alreadyStarted)
	if err != nil || passwordAttempt != password || playerCount >= playerCap || alreadyStarted {
		return false
	}

	query = "SELECT participantNumber FROM participants WHERE gameID = ?;"

	results, err = db.Query(query, gameID)
	if err != nil {
		return false
	}

	defer results.Close()

	var playerPositions = make([]bool, playerCap)

	for results.Next() {
		var playerNumber int

		err = results.Scan(&playerNumber)
		if err != nil {
			return false
		}

		if playerNumber < playerCap {
			playerPositions[playerNumber] = true
		}
	}

	var playerNumber int

	for i := 0; i < playerCap; i++ {
		if !playerPositions[i] {
			playerNumber = i
		}
	}

	_, err = db.Exec("INSERT INTO participants (participantID, gameID, participantNumber) VALUES (?, ?, ?);",
		userID, gameID, playerNumber)
	if err != nil {
		return false
	}

	_, err = db.Exec("UPDATE games SET playerCount = (SELECT COUNT(participantID) FROM participants WHERE gameID = ?) WHERE gameID = ?;", gameID, gameID)
	if err != nil {
		return false
	}

	return true
}

// returns a boolean representing whether the user could leave the game
func leaveGame(db *sql.DB, token string, gameID uint32) bool {
	userID, err := getID(db, token)
	if err != nil {
		return false
	}

	_, err = db.Exec("DELETE FROM participants WHERE participantID = ? AND gameID = ?", userID, gameID)
	if err != nil {
		return false
	}

	_, err = db.Exec("UPDATE games SET playerCount = (SELECT COUNT(participantID) FROM participants WHERE gameID = ?) WHERE gameID = ?;", gameID, gameID)
	if err != nil {
		return false
	}

	results, err := db.Query("SELECT hostID FROM games WHERE gameID = ?;", gameID)
	if err != nil {
		return false
	}

	if !results.Next() {
		return false
	}

	var hostID uint32

	results.Scan(&hostID)

	// if the host just left, either assign a new host or delete the game if no one's left
	if userID == hostID {
		nextHost, err := db.Query("SELECT participantID FROM participants WHERE gameID = ? ORDER BY participantNumber;")
		if err != nil {
			return false
		}

		if nextHost.Next() {
			var nextHostID uint32

			results.Scan(&nextHostID)

			_, err = db.Exec("UPDATE games SET hostID = ? WHERE gameID = ?;", nextHostID, gameID)
			if err != nil {
				return false
			}
		} else {
			err = deleteGame(db, gameID)
			if err != nil {
				return false
			}
		}
	}

	return true
}

// uploads a new or existing order to the game
func uploadOrder(db *sql.DB, token string, order Order) (Order, error) {
	if order.Timestamp < uint64(time.Now().UTC().Unix()) {
		return order, errors.New("Cannot send or update an order in the past")
	}

	nextUpdateTime = minInt(order.Timestamp, nextUpdateTime)

	id, err := getID(db, token)
	if err != nil {
		return order, err
	}

	// find the id that should be used to send the order
	query := "SELECT participantNumber FROM participants WHERE participantID = ? AND gameID = ?;"

	results, err := db.Query(query, id, order.GameID)
	if err != nil {
		return order, err
	}

	defer results.Close()
	if !results.Next() {
		return order, errors.New("SELECT query failed")
	}

	results.Scan(&order.SenderID)

	query = "REPLACE INTO orders (gameID, senderID, referenceID, type, parameters, FROM_UNIXTIME(timestamp)) VALUES (?, ?, ?, ?, ?);"

	res, err := db.Exec(query, order.GameID, order.SenderID, order.ReferenceID, order.Type, serialize(order.ArgumentIDs), order.Timestamp)
	if err != nil {
		return order, err
	}

	orderID, err := res.LastInsertId()
	if err != nil {
		return order, err
	}

	order.ID = uint32(orderID)

	return order, nil
}

// alert all players that the game state has changed
func distributeGameState(gameID uint32) {
	query := "SELECT participantID FROM users WHERE gameID = ?;"

	results, err := db.Query(query, gameID)
	if err != nil {
		fmt.Println(err)
		return
	}

	defer results.Close()
	for results.Next() {
		var id uint32

		if err = results.Scan(&id); err != nil {
			fmt.Println(err)
			return
		}

		if connection, ok := connections[id]; ok {
			if err := connection.WriteMessage(websocket.TextMessage, []byte("[GAMEUPDATE]"+strconv.FormatUint(uint64(gameID), 10))); err != nil {
				fmt.Println(err)
			}
		} else {
			outdatedGames[id] = true
		}
	}
}

func refreshPush(gameID uint32, pushToken string) {
	/*if len(pushText) > 100 {
		pushText = pushText[0:99]
	}

	payload := apns.NewPayload()
	payload.ContentAvailable = 1

	pn := apns.NewPushNotification()
	pn.DeviceToken = pushToken
	pn.AddPayload(payload)
	client := apns.NewClient("gateway.sandbox.push.apple.com:2195")
	client.Send(pn)

	_, _ = pn.PayloadString()*/
}
