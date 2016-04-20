// Handles Point specific code.

package maps

// Point is a struct for each point (X, Y) on the map
type Point struct {
	X int
	Y int
}

func (this Point) equals(point Point) bool {
	return this.X == point.X && this.Y == point.Y
}

// Returns true if this point is connected horizontaly or vertically to 'point'.
// @TODO: Work on diagonal support and cleaning up non-essential lines.
func (this Point) nextTo(point Point) bool {
	diagonalAllowed := false

	if diagonalAllowed {
		return (point.X >= this.X-1 && point.X <= this.X+1 && point.Y >= this.Y-1 && point.Y <= this.Y+1)
	}

	if this.X == point.X {
		return (this.Y-1 == point.Y || this.Y+1 == point.Y)
	} else if this.Y == point.Y {
		return (this.X-1 == point.X || this.X+1 == point.X)
	}
	return false
}
