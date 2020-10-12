// Netcat2 is a read-only TCP client with channels
package main

import (
	"log"
	"net"
	"os"
  "fmt"
  "strings"
)
func clockConection(conn net.Conn,) {
  for {
    hour := make([]byte,11)
    _, err := conn.Read(hour)
    if err != nil{
      conn.Close()
      log.Print(err)
    } else {
      fmt.Printf("\t%s", hour)
    }
  }
}

func main() {
  args := os.Args[1:]
  if(len (args) < 1){
    fmt.Println("Wrong parameters\nExpected input: go run clockWall.go City1=localhost:portNumber1 City2=localhost:portNumber2 ...")
    os.Exit(1)
  }
	for i := 0; i < len(args); i++ {
    connectionData := strings.Split(os.Args[i+1], "=")
    if len(connectionData) != 2 {
      fmt.Println("Wrong connection parameters")
    }
  	conn, err := net.Dial("tcp", connectionData[1])
    if err != nil {
      log.Fatal(err)
    }
    go clockConection(conn)
  }
  for {

  }
}
