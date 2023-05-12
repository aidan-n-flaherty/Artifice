package main

import (
	"database/sql"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"
	"github.com/go-co-op/gocron"
	"github.com/go-sql-driver/mysql"
	"github.com/gorilla/websocket"
)

var latestVersion = "1.1"

var db *sql.DB

var wsUpgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin:     func(r *http.Request) bool { return true },
}

var connections = make(map[uint32]*websocket.Conn)
var outdatedGames = make(map[uint32]bool)

const timeFormat = "20060102150405" // standard for MySQL timestamps

// this time represents the next time any game will update
var nextUpdateTime = time.Now().UTC().Format(timeFormat)

// maintain a websocket connection for a specific user
func processConnection(c *websocket.Conn, userID uint32, timeout time.Duration) {
    c.SetReadDeadline(time.Now().Add(timeout))

    c.SetPongHandler(func(data string) error {
        c.SetReadDeadline(time.Now().Add(timeout))
        return nil
    })

    // ping the client in intervals to make sure it's still there
    go func() {
        for {
            if err := c.WriteMessage(websocket.PingMessage, []byte{}); err != nil {
                break
            }

            time.Sleep(timeout/2)
        }
    }()

    // respond to messages sent by the client
    go func() {
        for {
            _, data, err := c.ReadMessage(); if err != nil {
                break
            }

            message := string(data[:])
            c.SetReadDeadline(time.Now().Add(timeout))

            if(strings.HasPrefix(message, "[READCHAT]")) {
                message = strings.TrimPrefix(message, "[READCHAT]")

                chatID, err := strconv.ParseUint(message, 10, 32); if err != nil {
                    fmt.Println(err)
                    continue
                }

                readChat(db, userID, uint32(chatID))
            } else {
                fmt.Println("Unexpected socket message: " + message)
            }
        }

        fmt.Println("Socket timed out")

        if(connections[userID] == c) {
            delete(connections, userID)
        }
        c.Close()
    }()
}

// run routine cron tasks to ensure that clients are up to date with game data
func clientUpdateLoop(){
    s := gocron.NewScheduler(time.UTC)

    // every second check if there are any orders that need to be sent to clients
    s.Every(1).Seconds().Do(func() {
        var currentTime = time.Now().UTC().Format(timeFormat)

        // only query the database when an order goes through
        if currentTime < nextUpdateTime {
            return
        }

        var gamesToUpdate = make(map[uint32]bool)
        query := "SELECT gameID FROM orders WHERE timestamp <= ?;"

        results, err := db.Query(query, currentTime); if err != nil {
            return
        }

        defer results.Close()
        for results.Next() {
            var gameID uint32

            err = results.Scan(&gameID); if err != nil {
                return
            }

            gamesToUpdate[gameID] = true
        }

        for gameID, _ := range gamesToUpdate {
            distributeGameState(gameID)
        }

        // move the order from orders to sentOrders
        query = "INSERT INTO sentOrders SELECT * FROM orders WHERE timestamp <= ?;"

        _, err = db.Exec(query, currentTime); if err != nil {
            return
        }

        query = "DELETE FROM orders WHERE timestamp <= ?;"

        _, err = db.Exec(query, currentTime); if err != nil {
            return
        }

        query = "SELECT MIN(timestamp) FROM orders;"

        results, err = db.Query(query); if err != nil {
            return
        }

        // find the next update time to wait for
        defer results.Close()
        if results.Next() {
            var nextTimestamp string

            results.Scan(&nextTimestamp)

            nextUpdateTime = nextTimestamp
        } else {
            nextUpdateTime = time.Now().UTC().Add(24 * time.Hour).Format(timeFormat)
        }
    })

    // every 20 minutes use background app refresh to alert clients of a new game state,
    // so clients can create local push notifications if necessary
    s.Every(20).Minutes().Do(func() {
        for gameID, _ := range outdatedGames {
            query := "SELECT token FROM participants INNER JOIN pushTokens ON participantID = userID WHERE gameID = ?;"

            results, err := db.Query(query, gameID); if err != nil {
                return
            }

            defer results.Close()
            for results.Next() {
                var pushToken string

                err = results.Scan(&pushToken); if err != nil {
                    return
                }

                refreshPush(gameID, pushToken)
                delete(outdatedGames, gameID)
            }
        }
    })

    // purge old tokens every 12 hours
    s.Every(12).Hours().Do(func() {
        query := "DELETE FROM auth WHERE TIMESTAMPDIFF(DAY, createdAt, NOW()) > 2;"

        _, err := db.Exec(query); if err != nil {
            return
        }
    })
}

func main() {
    var err error

    cfg := mysql.Config{
        User:   os.Getenv("DBUSER"),
        Passwd: os.Getenv("DBPASS"),
        Net:    "tcp",
        Addr:   "private-db-mysql-nyc1-88878-do-user-12821055-0.b.db.ondigitalocean.com:25060",
        DBName: "gamedata",
        Collation: "utf8mb4_unicode_ci",
    }

    db, err = sql.Open("mysql", cfg.FormatDSN()); if err != nil {
        log.Fatal(err)
    }

    db.SetConnMaxLifetime(time.Minute * 3)
    db.SetMaxOpenConns(10)
    db.SetMaxIdleConns(10)

    pingErr := db.Ping()
    if pingErr != nil {
        log.Fatal(pingErr)
	}

    fmt.Println("Connected!")

    // authentication
    http.HandleFunc("/ws", getWSConn)
    http.HandleFunc("/login", login)
    http.HandleFunc("/signup", signup)
    http.HandleFunc("/version", version)

    // users
    http.HandleFunc("/fetchUser", fetchUser)
    http.HandleFunc("/fetchSelf", fetchSelf)
    http.HandleFunc("/editSelf", editSelf)

    // game modification and data
    http.HandleFunc("/fetchGames", fetchGames)
    http.HandleFunc("/fetchUserGames", fetchUserGames)
    http.HandleFunc("/fetchGameState", fetchGameState)
    http.HandleFunc("/fetchGameDetails", fetchGameDetails)
    http.HandleFunc("/createMatch", createMatch)
    http.HandleFunc("/editMatch", editMatch)
    http.HandleFunc("/joinMatch", joinMatch)
    http.HandleFunc("/leaveMatch", leaveMatch)
    http.HandleFunc("/updateOrder", updateOrder)

    // user chats and messages
    http.HandleFunc("/fetchChat", fetchChat)
    http.HandleFunc("/fetchChats", fetchChats)
    http.HandleFunc("/fetchMessages", fetchMessages)
    http.HandleFunc("/sendMessage", sendMessage)
    http.HandleFunc("/reportMessage", reportMessage)
    http.HandleFunc("/block", block)

    fmt.Printf("Starting server at port 8080\n")
    if err := http.ListenAndServe(":8080", nil); err != nil {
        log.Fatal(err)
    }

    defer db.Close()

    clientUpdateLoop()
}
