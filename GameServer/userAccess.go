package main

import (
	"database/sql"
	"errors"
)

// Get profile information for the user whose session it is.
func getSelf(db *sql.DB, token string) (User, error) {
	var response User
	var err error

	response.ID, err = getID(db, token)
	if err != nil {
		return response, err
	}

	query := "SELECT username, pushEnabled, gamesWon, gamesLost, averageRank, rating, tier FROM users WHERE id = ?;"

	results, err := db.Query(query, response.ID)
	if err != nil {
		return response, err
	}

	defer results.Close()

	if !results.Next() {
		return response, errors.New("No results")
	}

	err = results.Scan(&response.Username, &response.PushEnabled, &response.Stats.GamesWon, &response.Stats.GamesLost,
		&response.Stats.AverageRank, &response.Stats.Rating, &response.Tier)

	if err != nil {
		return response, err
	}

	return response, nil
}

// get a specific user from their ID
func getUser(db *sql.DB, userID uint32) (User, error) {
	var response User
	response.ID = userID

	query := "SELECT username, gamesWon, gamesLost, averageRank, rating, tier FROM users WHERE id = ?;"

	results, err := db.Query(query, userID)
	if err != nil {
		return response, err
	}

	defer results.Close()

	if !results.Next() {
		return response, errors.New("No results")
	}

	err = results.Scan(&response.Username, &response.Stats.GamesWon, &response.Stats.GamesLost,
		&response.Stats.AverageRank, &response.Stats.Rating, &response.Tier)

	if err != nil {
		return response, err
	}

	return response, nil
}

func editUser(db *sql.DB, token string, user User) error {
	var err error

	user.ID, err = getID(db, token)
	if err != nil {
		return err
	}

	query := "UPDATE users SET username = ?, pushEnabled = ? WHERE id = ?;"

	_, err = db.Exec(query, user.Username, user.PushEnabled, user.ID)
	if err != nil {
		return err
	}

	return nil
}
