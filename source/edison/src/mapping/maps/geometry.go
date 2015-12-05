// Handles random geometry functions that aren't limited to a single shape.

package maps

import "fmt"
import "math"

const DEBUG bool = false

// Sin(degree) = opposite/distance.
func getOpposite(degree, distance float64) float64 {
	return distance * math.Sin(degreeToRadians(degree))
}

// Cos(degree) = adjacent/distance.
func getAdjacent(degree, distance float64) float64 {
	return distance * math.Cos(degreeToRadians(degree))
}

// Converts degrees into radians.
func degreeToRadians(degrees float64) float64 {
	return (degrees * math.Pi) / 180
}

// Converts radians into degrees.
func radianToDegrees(radians float64) float64 {
	return (radians * 180) / math.Pi
}

// Takes a one-dimensional array of binary values and converts it to an array of lines.
func BitmapToVector(floor [][]bool) []Line {	
	p_array := createPointArray(floor)

	// Converts Points Array into a Line Array. Possibly could be turned into stand alone function "pointsToLines(points []Point) []Line"
	l_array := pointsToLines(p_array[0:])

	// Adds a Slope to each Line object.
	for i := 0; i < len(l_array); i++ {
		l_array[i].Slope, l_array[i].Vertical = l_array[i].getSlope()
	}

	// Joins up any lines that have Points In Common and Equal Slope.
	for i := 0; i < len(l_array); i++ {
		for j := i + 1; j < len(l_array); j++ {
			if DEBUG {
				fmt.Println("== Line Array ==", i, j)
				PrintLineArray(l_array[0:])
			}
			if l_array[i].canJoin(l_array[j]) {
				l_array[i] = l_array[i].joinTo(l_array[j])
				l_array_temp := l_array[0:j]
				l_array = append(l_array_temp, l_array[j+1:]...)
				i, j = 0, 1
				break
			}
		}
	}

	return l_array
}

// Creates an array of lines out of an array of points.
func pointsToLines(points []Point) (lines []Line) {
	for i := 0; i < len(points); i++ {
		for j := i + 1; j < len(points); j++ {
			if points[i].nextTo(points[j]) {
				lines = append(lines, Line{P1: points[i], P2: points[j]})
			}
		}
	}
	return
}

// Converts a One-Dimensional Array of Binary values into Points, Returns a One-Dimensional Point Array
func createPointArray(array [][]bool) (points []Point) {
	height := len(array);
	width := len(array[0])

	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			if array[y][x] {
				points = append(points, Point{x % width, y})
			}
		}
	}
	return points[0:]
}
