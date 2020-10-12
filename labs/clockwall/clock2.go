// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"io"
	"log"
	"net"
	"time"
	"os"
	"fmt"
)

func handleConn(c net.Conn, timeZone string) {
	defer c.Close()
	for {
		_, timeErr := time.LoadLocation(timeZone)
		if timeErr != nil{
			fmt.Println("Error loading: "+ timeZone+" Timezone")
			log.Print(timeErr)
			break
		}
		_, err := io.WriteString(c, timeZone+" "+time.Now().Format("15:04:05\n"))
		if err != nil {
			log.Print(err)
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	args := os.Args[1:]
	if len(args) != 2 {
		fmt.Println("Wrong parameters\nExpected input: TZ=TimezoneHere go run clock2.go -port #portNumber")
		os.Exit(1)
	}
	localPort := "localhost:" + args[1]
	listener, err := net.Listen("tcp", localPort)
	if err != nil {
		log.Fatal(err)
	}
	timeZone := os.Getenv("TZ")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn, timeZone) // handle connections concurrently
	}
}
