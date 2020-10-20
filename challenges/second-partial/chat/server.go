// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"fmt"
	"log"
	"os"
	"net"
	"flag"
	"bufio"
	"bytes"
	"strings"
	"time"
)
//!+broadcaster
type client chan<- string // an outgoing message channel

type userStruct struct {
	userName string
	msg      string
}

type clientStruct struct {
	cliente    client
	connection net.Conn
	connectedSince string
}
var (
	serverPrefix = "irc-server > "
	admin        client
	globalUser   string
	entering     = make(chan clientStruct)
	leaving      = make(chan client)
	messages     = make(chan string) // all incoming client messages
	direct       = make(chan userStruct)
	kick         = make(chan string)
	users        map[string]client
	connections  map[string]net.Conn
	sinceConnections map[string]string
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	users = make(map[string]client)
	connections = make(map[string]net.Conn)
	sinceConnections = make(map[string]string)

	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case uS := <-direct:
			users[uS.userName] <- uS.msg

		case clientStructure := <-entering:
			if len(clients) == 0 {
				clientStructure.cliente <- serverPrefix + "Congrats, you were the first user."
				clientStructure.cliente <- serverPrefix + "You're the new IRC Server ADMIN"
				fmt.Printf("[%s] was promoted as the channel ADMIN\n", globalUser)
				admin = clientStructure.cliente
			}
			clients[clientStructure.cliente] = true
			users[globalUser] = clientStructure.cliente
			connections[globalUser] = clientStructure.connection
			sinceConnections[globalUser] = clientStructure.connectedSince

		case cli := <-leaving:
			if admin == cli {

				for newAdmin := range clients {
					admin = newAdmin
					newAdmin <- serverPrefix + "You're the new admin!"
					continue
				}
			}

			delete(clients, cli)
			close(cli)
		case user := <-kick:
			connection := connections[user]
			client := users[user]
			delete(clients, client)
			delete(connections, user)
			delete(users, user)
			delete(sinceConnections, user)
			connection.Close()
		}
	}
}

func handleConn(conn net.Conn) {
	//GET THE USER NAME
	var buf = make([]byte, 1024)
	conn.Read(buf)
	localUser := string(bytes.Trim(buf, "\x00"))
	globalUser = string(bytes.Trim(buf, "\x00"))

	ch := make(chan string) // outgoing client messages

	if users[localUser] != nil {
		// USER ALREADY EXIST
		fmt.Fprintln(conn, "User with name: "+localUser+" already exists")
		close(ch)
		conn.Close()
		return
	}
	fmt.Printf("%sNew connected user [%s]\n", serverPrefix, localUser)
	go clientWriter(conn, ch)

	// WELCOME MESSAGE
	ch <- serverPrefix + "Welcome to the Simple IRC Server"
	ch <- serverPrefix + "Your user [" + localUser + "] is successfully logged"

	// ENTERING MESSAGES
	messages <- serverPrefix + "New connected user [" + localUser + "]"
	entering <- clientStruct{ch, conn, time.Now().Format("15:04:05\n")}

	input := bufio.NewScanner(conn)
	for input.Scan() {
		if len(input.Text()) > 0 && string(input.Text()[0]) == "/" {
			slice := strings.Split(input.Text(), " ")
			command := slice[0]
			switch command {
			// USERS COMMAND
			case "/users":
				str := ""
				for usuario := range users {
					userConnectedSince := sinceConnections[usuario]
					str += serverPrefix + usuario + " - connected since " + userConnectedSince
				}
				ch <- str
			// MESSAGE COMMAND
			case "/msg":
				if len(slice) < 2 {
					ch <- "Please specify a user"
					continue
				}
				if len(slice) < 3 {
					ch <- "Please enter a message"
					continue
				}
				addressee := slice[1]
				if _, ok := users[addressee]; ok {
					directMessage := input.Text()[strings.Index(input.Text(), addressee)+len(addressee)+1:]
					direct <- userStruct{addressee, localUser + " > " + directMessage}
				} else {
					ch <- "User: " + addressee + " doesn't exist"
				}
			// TIME COMMAND
			case "/time":
				timezone := "America/Mexico_City"
				loc, _ := time.LoadLocation(timezone)
				theTime := time.Now().In(loc).Format("15:04\n")
				ch <- serverPrefix + "Local Time: " + timezone + " " + strings.Trim(theTime, "\n")
			// USER COMMAND
			case "/user":
				if len(slice) < 2 {
					ch <- "Please enter a user"
					continue
				}
				user := slice[1]
				if _, ok := users[user]; ok {
					ip := connections[user].RemoteAddr().String()
					userConnectedSince := sinceConnections[user]
					ch <- serverPrefix + "username: " + user + ", IP: " + ip + " - connected since " + userConnectedSince

				} else {
					ch <- "User: " + user + " doesn't exist"
				}
			// KICK COMMAND
			case "/kick":
				if len(slice) < 2 {
					ch <- "Please enter a user to kick"
					continue
				}
				if ch == admin {
					user := slice[1]
					if _, ok := users[user]; ok {
						messages <- "[" + user + "] was kicked from channel"
						fmt.Printf("%s[%s] was kicked from channel\n", serverPrefix, user)
						// MESSAGE TO THE USER KICKED
						direct <- userStruct{user, serverPrefix + "Youre kicked from this channel"}
						kick <- user
					} else {
						ch <- "User: " + user + " doesn't exist"
					}
				} else {
					ch <- "Only the admin can kick people out of the server"
				}
			default:
				ch <- "Invalid command"
			}
		} else {
			messages <- localUser + " > " + input.Text()
		}
	}
	// When user left chat
	leaving <- ch
	messages <- "[" + localUser + "] has left" // clients message
	fmt.Printf("%s[%s] left\n", serverPrefix, localUser) // server message
	delete(users, localUser)
	delete(connections, localUser)
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}



// main
func main() {
	if len(os.Args) < 5 {
		fmt.Println("usage: go run server.go -host localhost -port [port]")
		os.Exit(1)
	}
	host := flag.String("host", "localhost", "localhost")
	port := flag.String("port", "9000", "the port")
	flag.Parse()
	listener, err := net.Listen("tcp", *host+":"+*port)
	fmt.Println(serverPrefix + "Simple IRC Server started at " + *host + ":" + *port)
	if err != nil {
		log.Fatal(err)
	}
	go broadcaster()
	fmt.Println(serverPrefix + "Ready for receiving new clients")
	for {

		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}
// main
