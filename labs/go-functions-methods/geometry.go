// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
		"fmt"
		"math"
		"math/rand"
		"os"
		"strconv"
	)

type Point struct{ X, Y float64 }
func newPoint() Point {
	newPoint := Point{}
	newPoint.setX(rand.Float64()*(101+101) - 101)
	newPoint.setY(rand.Float64()*(101+101) - 101)
	return newPoint
}
func (p *Point) setX(x float64){
	p.X = x
}
func (p *Point) setY(y float64){
	p.Y = y
}
func (p *Point) getX() float64 {
	return p.X
}
func (p *Point) getY() float64 {
	return p.Y
}
// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
func onSegment(p, q, r Point) bool {
	if q.getX() <= math.Max(p.getX(), r.getX()) && q.getX() >= math.Min(p.getX(), r.getX()) && q.getY() <= math.Max(p.getY(), r.getY()) && q.getY() >= math.Min(p.getY(), r.getY()) {
		return true
	}
	return false
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q, and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
func orientation(p, q, r Point) int {
	val := (q.getY()-p.getY())*(r.getX()-q.getX()) - (q.getX()-p.getX())*(r.getY()-q.getY())

	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 2
}
// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect
func doIntersect(p1, q1, p2, q2 Point) bool {
	// Find the four orientations needed for general and
	// special cases
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)
	// General case
	if o1 != o2 && o3 != o4 {
		return true
	}
	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if o1 == 0 && onSegment(p1, p2, q1) {
		return true
	}
	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if o2 == 0 && onSegment(p1, q2, q1) {
		return true
	}
	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if o3 == 0 && onSegment(p2, p1, q2) {
		return true
	}
	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if o4 == 0 && onSegment(p2, q1, q2) {
		return true
	}
	return false // Doesn't fall in any of the above cases
}
// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	sum += path[0].Distance(path[len(path)-1])
	return sum
}

func shapeCheck(path Path) bool {
	for i := 0; i < len(path); i++ {
		if doIntersect(path[i%len(path)], path[(i+1)%len(path)], path[(i+2)%len(path)], path[(i+3)%len(path)]) == true {
			return false
		}
	}
	for i := 1; i < len(path)-3; i++ {
		if doIntersect(path[len(path)-1], path[0], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	for i := 0; i < len(path)-4; i++ {
		if doIntersect(path[len(path)-1], path[len(path)-2], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	return true
}

func Shape(nPoints int) Path {
	var path Path

	for i := 0; i < nPoints; i++ {
		path = append(path, newPoint())
		if i == 2 {
			for orientation(path[0], path[1], path[2]) == 0 {
				path[2] = newPoint()
			}
		} else if i > 2 {
			for shapeCheck(path) == false {
				path[i] = newPoint()
			}
		}

	}
	return path
}
func main() {
	args := os.Args[1:]
	if len(args) < 1 || len(args) > 1 {
		fmt.Println("Wrong number of parameters")
		return
	}
	vertex, err := strconv.Atoi(args[0])
	if err != nil {
		fmt.Println("Cannot convert string to number")
		return
	} else if vertex < 3 {
		fmt.Println("Need at least 3 points to create a Figure")
		return
	}
	fmt.Println("Vertices")
	shape := Shape(vertex)

	for i := 0; i < len(shape); i++ {
		fmt.Printf("\t(%f,\t%f)\n", shape[i].getX(), shape[i].getY())
	}
	fmt.Println("Perimeter")
	for i := range shape {
		if i > 0 && i+1 < len(shape) {
			fmt.Printf("+%f", shape[i].Distance(shape[i+1]))
		} else if i == 0 {
			fmt.Printf("%f", shape[i].Distance(shape[i+1]))
		}
	}
	fmt.Printf("+%f\n = %f\n", shape[0].Distance(shape[len(shape)-1]), shape.Distance())
}
//!-path
