package main

type jsonObj map[string]interface{}

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
	CreatedAt   uint64 `json:"createdAt"`
	PlayerCount uint   `json:"playerCount"`
	StartTime   uint64 `json:"startTime"`
	HasPassword bool   `json:"hasPassword"`
	Version     string `json:"version"`
}

type GameSettings struct {
	LobbyName             string                 `json:"lobbyName"`
	Password              string                 `json:"password"`
	PlayerCap             uint                   `json:"playerCap"`
	StartTimeDisplacement uint64                 `json:"startTime"`
    SettingOverrides      jsonObj                `json:"settingOverrides"`
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
	ReferenceID int32    `json:"referenceID"`
	Type        string   `json:"type"`
	ArgumentIDs []uint32 `json:"argumentIDs"`
	Timestamp   uint64   `json:"timestamp"`
}

type PushToken struct {
	Token  string `json:"token"`
	UserID uint32 `json:"userID"`
}

type Chat struct {
	ID            uint32    `json:"id"`
	Participants  []uint32  `json:"participants"`
	Messages      []Message `json:"messages"`
	ReadTimestamp uint64    `json:"readTimestamp"`
}

type Message struct {
	ID        uint32 `json:"id"`
	SenderID  uint32 `json:"senderID"`
	Content   string `json:"content"`
	Timestamp uint64 `json:"timestamp"`
}
