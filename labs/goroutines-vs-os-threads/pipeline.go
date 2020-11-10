package main

import (
	"fmt"
	"time"
)

var waitingChannel = make(chan struct{})
var transitTime = time.Now()

func pipeline(one chan struct{}, two chan struct{}, currentStages int, maxStages int) {
	if currentStages <= maxStages {
		fmt.Println("Maximum number of pipeline stages   : ", currentStages)
		fmt.Println("Time to transit trough the pipeline : ", time.Since(transitTime))
    transitTime = time.Now()
		go pipeline(two, one, currentStages+1, maxStages)
		two <- <-one
	} else {
    fmt.Println("Maximum number of stages reached")
		waitingChannel <- struct{}{}
	}
}

func main() {
	var pipeOne chan (struct{})
	var pipeTwo chan (struct{})
	//go pipeline(pipeOne, pipeTwo, 0, 1941558)
	go pipeline(pipeOne, pipeTwo, 0, 1000000)
	<-waitingChannel

}
