package main

import (
	"database/sql"
	"errors"
	"fmt"
	"math/rand"
	"strconv"

	"golang.org/x/crypto/bcrypt"
)

// Return a temporary token to use for all session requests.
// More secure than sending login credentials with each request.
func getToken(db *sql.DB, id uint32, password string) (uint64, error) {
	if invalidAccess(db, id, password) {
		return 0, errors.New("Invalid access")
	}

	var notUnique = true
	var uniqueKey uint64

	// loop until a unique key is obtained, since collisions are unlikely
	for notUnique {
		uniqueKey = rand.Uint64()
		query := "SELECT EXISTS(SELECT 1 FROM auth WHERE token = ? LIMIT 1);"

		results, err := db.Query(query, uniqueKey)
		if err != nil {
			return 0, err
		}

		defer results.Close()
		if !results.Next() {
			return 0, errors.New("Token access failed.")
		}

		// for each row, scan the result into our tag composite object
		err = results.Scan(&notUnique)
		if err != nil {
			return 0, err
		}
	}

	str := "INSERT INTO auth (token, id) VALUES(?, ?);"

	_, err := db.Exec(str, uniqueKey, id)
	if err != nil {
		return 0, err
	}

	return uniqueKey, nil
}

// Get a user ID associated with a temporary session token
// DO NOT use tokens more than 48 hours old, as they may be purged
func getID(db *sql.DB, sessionID string) (uint32, error) {
	var id uint32
	query := "SELECT id FROM auth WHERE token = ? LIMIT 1;"

	sID, err := strconv.ParseUint(sessionID, 10, 64)
	if err != nil {
		return 0, err
	}

	results, err := db.Query(query, sID)
	if err != nil {
		return 0, err
	}

	defer results.Close()
	if !results.Next() {
		return 0, errors.New("No results")
	}

	err = results.Scan(&id)
	if err != nil {
		fmt.Println("Invalid token")
		return 0, err
	}

	return id, nil
}

// given an ID and a password, check if they match
func invalidAccess(db *sql.DB, id uint32, password string) bool {
	var userPassword []byte

	query := "SELECT password FROM users WHERE id = ?;"

	results, err := db.Query(query, id)
	if err != nil {
		return true
	}

	defer results.Close()
	if !results.Next() {
		return true
	}

	err = results.Scan(&userPassword)
	if err != nil {
		return true
	}

	return bcrypt.CompareHashAndPassword(userPassword, []byte(password)) != nil
}

// check if a user cannot access a chat
func invalidChatAccess(db *sql.DB, userID uint32, chatID uint32) bool {
	query := "SELECT EXISTS (SELECT userID FROM chatAccess WHERE userID = ? AND chatID = ?);"

	results, err := db.Query(query, userID, chatID)
	if err != nil {
		return true
	}

	var hasAccess bool
	err = results.Scan(&hasAccess)
	if err != nil {
		return true
	}

	return !hasAccess
}

// Generate a hash from a string password.
// If a hacker gets read access to users, then they still won't be able to log in.
func hashPassword(password string) ([]byte, error) {
	hashedPassword, err := bcrypt.GenerateFromPassword([]byte(password), 8)

	return hashedPassword, err
}

// this function should be called whenever the user logs into a new device
func addPushToken(db *sql.DB, token string, pushToken string) {
	id, err := getID(db, token)
	if err != nil {
		fmt.Println(err)
		return
	}

	// if this token was used by another account before, ownership will be changed
	_, err = db.Exec("DELETE FROM pushTokens WHERE userID = ?;", id)
	if err != nil {
		fmt.Println(err)
		return
	}

	_, err = db.Exec("INSERT INTO pushTokens (token, userID) VALUES(?, ?);", pushToken, id)
	if err != nil {
		fmt.Println(err)
		return
	}
}
