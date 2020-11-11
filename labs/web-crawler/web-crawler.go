// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
  "fmt"
	"log"
	"os"
	"time"
  "bufio"
	"flag"
	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

func crawl(link node) []node {
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(link.url)
	<-tokens // release the token

	if err != nil {
		// Disabled verbose logs because screen gets cluttered with them during tests
		// Uncomment if you wish to see them
		// log.Print(err)
	}
	next := make([]node, 0, len(list))
	for _, val := range list {
		next = append(next, node{val, link.depth + 1})
	}
	return next
}

//!-sema

type node struct {
	url   string
	depth int
}
func displayProgress() {
	states := []string{"|", "/", "-", "\\"}
	for i := 0; ; i = (i + 1) % len(states) {
		time.Sleep(100 * time.Millisecond)
		fmt.Printf("\r\033[K\r%s", states[i])
	}
}
//!+
func main() {
	if len(os.Args) < 4 {
		log.Println("Usage: ./web-crawler -depth=DEPTH -results=FILENAME URL")
		return
	}
	depth := flag.Int("depth", 0, "Web crawler depth")
	results := flag.String("results", "results.txt", "Output file")
	flag.Parse()
	fmt.Printf("Web Crawler\nLink: %s\nDepth: %d\n", os.Args[3], *depth)
	// Create output file and initialize buffered writer
	f, err := os.Create(*results)
	if err != nil {
		panic(err)
	}
	w := bufio.NewWriter(f)
	defer w.Flush()
	worklist := make(chan []node)
	var n int // number of pending sends to worklist
	// Start with the command-line arguments.
	n++
	go func() {
		worklist <- []node{node{os.Args[3], 0}}
	}()
	currentDepth := -1
	visited := make(map[string]bool)
	go displayProgress()
	for ; n > 0; n-- {
		list := <-worklist
		for _, link := range list {
			if !visited[link.url] {
				if link.depth > currentDepth {
					currentDepth = link.depth
				}
				visited[link.url] = true
				fmt.Fprintf(w, "%s\n", link.url)
				if link.depth+1 > *depth {
					continue
				}
				n++
				go func(link node) {
					worklist <- crawl(link)
				}(link)
			}
		}
	}
  fmt.Printf("\r\033[K\rCompleted")
	fmt.Printf("\nOutput file is ready and named as %s\n", *results)
}
//!-
