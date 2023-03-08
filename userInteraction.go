package main

import (
	"database/sql"
	"errors"
	"fmt"
	"strconv"
	"time"

	"github.com/gorilla/websocket"
)

// start a new chat within a game with the users specified
func createChat(db *sql.DB, token string, gameID uint32, userIDs []uint32) error {
	userID, err := getID(db, token)
	if err != nil {
		return err
	}

	userIDs = append(userIDs, userID)

	str := "INSERT INTO chats (gameID) VALUES(?);"

	result, err := db.Exec(str, gameID)
	if err != nil {
		return err
	}

	chatID, err := result.LastInsertId()
	if err != nil {
		return err
	}

	// alert all websocket connections of chat creation
	for _, id := range userIDs {
		if connection, ok := connections[id]; ok {
			if err := connection.WriteMessage(websocket.TextMessage, []byte("[CHAT]"+strconv.FormatUint(uint64(chatID), 10))); err != nil {
				fmt.Println(err)
			}
		}
	}

	return nil
}

// fetches a specific chat given the id
func getChat(db *sql.DB, token string, chatID uint32) (Chat, error) {
	var chat = Chat{
		ID:           chatID,
		Participants: make([]uint32, 0),
		Messages:     make([]Message, 0),
	}

	id, err := getID(db, token)
	if err != nil {
		return chat, err
	}

	query := "SELECT readTimestamp FROM chatAccess c WHERE chatID = ? AND participantID = ?;"

	results, err := db.Query(query, chatID, id)
	if err != nil {
		return chat, err
	}

	defer results.Close()
	if !results.Next() {
		return chat, errors.New("Invalid access")
	}

	if err = results.Scan(&chat.ReadTimestamp); err != nil {
		return chat, err
	}

	query = "SELECT participantID FROM chatAccess c WHERE chatID = ? AND participantID != ?;"

	results, err = db.Query(query, chatID, id)
	if err != nil {
		return chat, err
	}

	defer results.Close()
	for results.Next() {
		var id uint32

		err = results.Scan(&id)
		if err != nil {
			return chat, err
		}

		chat.Participants = append(chat.Participants, id)
	}

	chat.Messages, err = getMessages(db, token, chatID, 0)
	if err != nil {
		return chat, err
	}

	return chat, nil
}

// fetches all chats for the user in question
func getChats(db *sql.DB, token string, gameID uint32) ([]Chat, error) {
	chats := make([]Chat, 0)

	userID, err := getID(db, token)
	if err != nil {
		return chats, err
	}

	query := "SELECT chatID FROM chatAccess WHERE gameID = ? AND participantID = ?;"

	results, err := db.Query(query, userID)
	if err != nil {
		return chats, err
	}

	defer results.Close()
	for results.Next() {
		var id uint32

		err = results.Scan(&id)
		if err != nil {
			return chats, err
		}

		chat, err := getChat(db, token, id)
		if err != nil {
			return chats, err
		}

		chats = append(chats, chat)
	}

	return chats, nil
}

func readChat(db *sql.DB, userID uint32, chatID uint32) bool {
	str := "UPDATE chatAccess SET readTimestamp = NOW() WHERE userID = ? AND chatID = ?;"

	_, err := db.Exec(str, userID, chatID)
	if err != nil {
		fmt.Println(err)
		return false
	}

	return true
}

// get the next 20 messages in a chat after a certain offset, offset should be the number of
// messages that the client currently has already loaded
func getMessages(db *sql.DB, token string, chatID uint32, offset int) ([]Message, error) {
	messages := make([]Message, 0)

	userID, err := getID(db, token)
	if err != nil {
		return messages, err
	}

	query := "SELECT id, senderID, content, timestamp FROM messages WHERE chatID = ? AND senderID NOT IN (SELECT blockedID FROM blocked WHERE userID = ?) ORDER BY timestamp DESC LIMIT ?, 20;"

	results, err := db.Query(query, chatID, userID, offset)
	if err != nil {
		return messages, err
	}

	defer results.Close()
	for results.Next() {
		var message Message

		err = results.Scan(&message.ID, &message.SenderID, &message.Content, &message.Timestamp)
		if err != nil {
			return messages, err
		}

		messages = append(messages, message)
	}

	return messages, nil
}

// flagged messages will have to be scanned routinely to identify bannable behavior
func flagMessage(db *sql.DB, token string, messageID uint32) error {
	reporterID, err := getID(db, token)
	if err != nil {
		return err
	}

	str := "INSERT INTO flaggedMessages (messageID, reporterID) VALUES(?, ?);"

	_, err = db.Exec(str, messageID, reporterID)
	if err != nil {
		return err
	}

	return nil
}

// users cannot receive anymore messages from a user they've blocked
func blockUser(db *sql.DB, token string, blockID uint32) error {
	userID, err := getID(db, token)
	if err != nil {
		return err
	}

	str := "INSERT INTO blocked (userID, blockedID) VALUES(?, ?);"

	_, err = db.Exec(str, userID, blockID)
	if err != nil {
		return err
	}

	return nil
}

// send a text message to a chat, currently only supports text
func sendTextMessage(db *sql.DB, token string, chatID uint32, content string) error {
	var message Message

	senderID, err := getID(db, token)
	if err != nil {
		return err
	}

	if invalidChatAccess(db, senderID, chatID) {
		return errors.New("Invalid access.")
	}

	str := "INSERT INTO messages (senderID, chatID, content) VALUES(?, ?, ?);"

	result, err := db.Exec(str, senderID, chatID, content)
	if err != nil {
		return err
	}

	message = Message{
		Content:  content,
		SenderID: senderID,
	}

	if id, err := result.LastInsertId(); err == nil {
		message.ID = uint32(id)
	}

	message.Timestamp = time.Now().UTC().Format(timeFormat)

	distributeMessage(senderID, chatID, message)

	return nil
}

// send message to websocket if possible, otherwise send a push notification
func distributeMessage(userID uint32, chatID uint32, message Message) {
	query := "SELECT username FROM profiles WHERE id = ?;"

	results, err := db.Query(query, userID)
	if err != nil {
		fmt.Println(err)
		return
	}

	defer results.Close()
	if !results.Next() {
		return
	}

	var name string
	results.Scan(&name)

	// only send messages to users that have not blocked this user
	query = "SELECT participantID FROM chatAccess WHERE chatID = ? AND NOT EXISTS(SELECT blockedID FROM blocked WHERE userID = participantID && blockedID = ?);"

	results, err = db.Query(query, chatID, userID)
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
			if err := connection.WriteJSON(message); err != nil {
				fmt.Println(err)
			}
		} else {
			query = "SELECT token FROM pushTokens WHERE id = ?;"

			tokenResults, err := db.Query(query, userID)
			if err != nil {
				fmt.Println(err)
			}

			defer tokenResults.Close()
			for tokenResults.Next() {
				var pushToken string

				if err = tokenResults.Scan(&pushToken); err != nil {
					fmt.Println(err)
					continue
				}

				pushNotification(name+": "+message.Content, pushToken)
			}
		}
	}
}

func pushNotification(pushText string, pushToken string) {
	if pushToken == "" {
		return
	}

	/*if len(pushText) > 100 {
		pushText = pushText[0:99]
	}

	payload := apns.NewPayload()
	payload.Alert = pushText

	pn := apns.NewPushNotification()
	pn.DeviceToken = pushToken
	pn.AddPayload(payload)
	client := apns.NewClient("gateway.sandbox.push.apple.com:2195")
	client.Send(pn)

	_, _ = pn.PayloadString()*/
}
