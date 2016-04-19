// Handles all Line specific methods.

package maps

import (
	"fmt"
	"math"
)

type Line struct {
	P1       Point
	P2       Point
	Vertical bool
	Slope    int
}

// Gets the slope of the line. Returns (slope int, vertical bool). If vertical, Slope will be set to 0.
func (this Line) getSlope() (int, bool) {
	if this.P1.X == this.P2.X {
		return 0, true
	}
	return ((this.P1.Y - this.P2.Y) / (this.P1.X - this.P2.X)), false
}

func (this Line) getAngleFrom(point Point) int {
	if this.pointOnLine(point) {
		if this.Vertical {
			if this.getOtherPoint(point).Y < point.Y {
				return 0
			}
			return 180
		} else {
			if this.getOtherPoint(point).X < point.X {
				return 270
			}
			return 90
		}
	}
	fmt.Println("[Point::getAngleFrom] Attempting to get angle for a point thats not on the line...")
	return 0
}

func (this Line) getLength() int {
	a := float64((this.P1.X - this.P2.X) * (this.P1.X - this.P2.X))
	b := float64((this.P1.Y - this.P2.Y) * (this.P1.Y - this.P2.Y))

	slope, _ := this.getSlope()

	if this.Vertical {
		return int(math.Abs(float64(this.P1.Y-this.P2.Y))) * BitmapScale
	} else if slope == 0 {
		return int(math.Abs(float64(this.P1.X-this.P2.X))) * BitmapScale
	}

	return int(math.Sqrt(a+b)) * BitmapScale
}

func (this Line) pointOnLine(point Point) bool {
	return this.P1.equals(point) || this.P2.equals(point)
}

func (this Line) getOtherPoint(point Point) Point {
	if this.pointOnLine(point) {
		if this.P1.equals(point) {
			return this.P2
		}

		if this.P2.equals(point) {
			return this.P1
		}
	}
	return point
}

// Returns true if the lines have a point in common. (Not Point of Intersection)
func (this Line) pointInCommon(line Line) bool {
	return (this.P1 == line.P1 || this.P1 == line.P2 || this.P2 == line.P1 || this.P2 == line.P2)
}

// Checks if the 2 can be joined into 1 line.
func (this Line) canJoin(line Line) bool {
	if this.pointInCommon(line) {
		if this.Vertical && line.Vertical {
			return true
		} else if this.Vertical || line.Vertical {
			return false
		} else if this.Slope == line.Slope {
			return true
		}
	}
	return false
}

// Joins 2 lines into a single line. Returns joined line.
func (this Line) joinTo(line Line) (new_line Line) {
	new_line.Slope, new_line.Vertical = this.Slope, this.Vertical

	if this.P1 == line.P1 {
		new_line.P1, new_line.P2 = this.P2, line.P2
	} else if this.P1 == line.P2 {
		new_line.P1, new_line.P2 = this.P2, line.P1
	} else if this.P2 == line.P1 {
		new_line.P1, new_line.P2 = this.P1, line.P2
	} else if this.P2 == line.P2 {
		new_line.P1, new_line.P2 = this.P1, line.P1
	}

	return
}

// PrintLineArray Prints out the array of lines in a nice way.
func PrintLineArray(array []Line) {
	for i := 0; i < len(array); i++ {
		fmt.Println(array[i].P1, "...", array[i].P2, array[i].Vertical, array[i].Slope)
	}
}
