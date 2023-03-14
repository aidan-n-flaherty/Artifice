package main

type User struct {
	ID          uint32    `json:"id"`
	Username    string    `json:"username"`
	Password    string    `json:"password"`
	PushEnabled bool      `json:"pushEnabled"`
	Stats       UserStats `json:"userStats"`
	Tier        uint      `json:"tier"`
}

type UserStats struct {
	GamesWon    uint    `json:"gamesWon"`
	GamesLost   uint    `json:"gamesLost"`
	AverageRank float64 `json:"averageRank"`
	Rating      uint    `json:"rating"`
}

type GameData struct {
	ID          uint32 `json:"id"`
	HostID      uint32 `json:"hostID"`
	CreatedAt   string `json:"createdAt"`
	PlayerCount uint   `json:"playerCount"`
	HasPassword bool   `json:"hasPassword"`
	Version     string `json:"version"`
}

type GameSettings struct {
	LobbyName         string   `json:"lobbyName"`
	Password          string   `json:"password"`
	PlayerCap         uint     `json:"playerCap"`
	RatingConstraints []uint   `json:"ratingConstraints"`
	StartTime         string   `json:"startTime"`
	SimulationSpeed   float64  `json:"simulationSpeed"`
	Weekdays          []string `json:"weekdays"`
	ActiveTimes       []string `json:"activeTimes"`
}

type GameDetails struct {
	Data     GameData     `json:"gameData"`
	Settings GameSettings `json:"gameSettings"`
}

type GameState struct {
	Orders  []Order      `json:"orders"`
	Players map[int]User `json:"users"`
}

type Order struct {
	ID          uint32   `json:"id"`
	SenderID    uint32   `json:"senderID"`
	ReferenceID uint32   `json:"referenceID"`
	GameID      uint32   `json:"gameID"`
	Type        string   `json:"type"`
	ArgumentIDs []uint32 `json:"argumentIDs"`
	Timestamp   string   `json:"timestamp"`
}

type PushToken struct {
	Token  string `json:"token"`
	UserID uint32 `json:"userID"`
}

type Chat struct {
	ID            uint32    `json:"id"`
	Participants  []uint32  `json:"participants"`
	Messages      []Message `json:"messages"`
	ReadTimestamp string    `json:"readTimestamp"`
}

type Message struct {
	ID        uint32 `json:"id"`
	SenderID  uint32 `json:"senderID"`
	Content   string `json:"content"`
	Timestamp string `json:"timestamp"`
}
