package main

import (
	"fmt"
	"time"
  "math"
	"os"
)
func main() {
	channel := make(chan struct{})
	msg := make(chan int)
	var commsPerSecond int
	timeStart := time.Now()
	go func() {
		msg <- 1
		for {
			commsPerSecond++
			msg <- <-msg
		}
		channel <- struct{}{}
	}()
	go func() {
		for {
			commsPerSecond++
			msg <- <-msg
		}
		channel <- struct{}{}
	}()
	for {
		select {
		case <-time.After(1 * time.Second):
      x := float64(commsPerSecond)/time.Since(timeStart).Seconds()
			fmt.Println("Communications Per Second : ", math.Round(x))
		}
		if commsPerSecond > 100000000 {
			fmt.Println("End of Communication")
			os.Exit(0)
		}
	}
	<-channel
}
