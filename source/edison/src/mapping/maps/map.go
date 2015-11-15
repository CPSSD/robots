// Handles all the mapping code.

package maps

import "fmt"
import "math"

const SIZE = 1 // Unused so far. Will implement it soon.

var RobotMap Map

type Map struct {
	width       int
	height      int
	last_width  int
	last_height int

	robot Robot

	floor [][]bool
	seen  [][]bool
}

func (this *Map) GetBitmap() [][]bool {
	return this.floor
}

// Returns the maps robot.
func (this *Map) GetRobot() Robot {
	return this.robot
}

// Creates an empty 1x1 (this single point will be the robots starting position.)
func CreateMap() (createdMap Map) {
	createdMap = Map{width: 1, height: 1, robot: Robot{0.5, 0.5, 0}}
	createdMap.floor = append(createdMap.floor, []bool{false})
	createdMap.seen = append(createdMap.seen, []bool{false})
	return
}

// Moves the robot along the path.
func (this *Map) MoveRobotAlongPath(path [][]bool) {
	prevX, prevY := -1, -1
	possibleMove := true
	nextX, nextY := 0, 0
	for possibleMove {
		nextX, nextY, possibleMove = this.getNextMove(int(this.robot.x), int(this.robot.y), prevX, prevY, path)

		prevX, prevY = int(this.robot.x), int(this.robot.y)
		this.robot.MoveToPoint(nextX, nextY)
	}
	fmt.Println("Finished Moving along path.")
}

// Returns the next move in the path.
func (this *Map) getNextMove(x, y, prevX, prevY int, path [][]bool) (x1 int, y1 int, possibleMove bool) {
	if x-1 != prevX && !(x-1 < 0 || x-1 >= this.width) {
		if path[y][x-1] {
			return x - 1, y, true
		}
	}
	if x+1 != prevX && !(x+1 < 0 || x+1 >= this.width) {
		if path[y][x+1] {
			return x + 1, y, true
		}
	}
	if y-1 != prevY && !(y-1 < 0 || y-1 >= this.height) {
		if path[y-1][x] {
			return x, y - 1, true
		}
	}
	if y+1 != prevY && !(y+1 < 0 || y+1 >= this.height) {
		if path[y+1][x] {
			return x, y + 1, true
		}
	}
	return x, y, false
}

// Adds a wall in position (x, y) of the map. Expands if neccesary
func (this *Map) AddWall(x, y int) {
	tempMap := this

	if !this.pointInMap(x, y) {
		expandX, expandY := 0, 0
		tempMap, expandX, expandY = this.expandMap(x, y)
		if expandX < 0 {
			expandX = -expandX
		} else {
			expandX = 0
		}

		if expandY < 0 {
			expandY = -expandY
		} else {
			expandY = 0
		}

		tempMap.robot.x += float64(expandX)
		tempMap.robot.y += float64(expandY)
		tempMap.seen[y+expandY][x+expandX] = true
		tempMap.floor[y+expandY][x+expandX] = true
	} else {
		tempMap.seen[y][x] = true
		tempMap.floor[y][x] = true
	}
	this = tempMap
}

// Scans an area with lasers. Adding all walls detected to the map.
func (this *Map) ScanArea() {
	// Integrate with Darraghs Code to send a command to scan an area.
	// * Blocked until Simulator works.
}

// Returns a boolean denoting whether the point exists inside the map.
func (this *Map) pointInMap(x, y int) bool {
	return !(x < 0 || x >= this.width || y < 0 || y >= this.height)
}

// Decides how much the map should expand by. returns: expanded map, expandX, expandY
func (this *Map) expandMap(x, y int) (*Map, int, int) {
	expandX, expandY := x, y

	if x >= this.width {
		expandX = x + 1 - this.width
	}
	if y >= this.height {
		expandY = y + 1 - this.height
	}

	return this.createExpandedMap(expandX, expandY), expandX, expandY

}

// Creates the new expanded map and fills it with the contents of the current map.
func (this *Map) createExpandedMap(expandX, expandY int) (tempMap *Map) {
	tempMap = this

	// Expands along X axis.
	for i := 0; i < tempMap.height; i++ {
		if expandX > 0 {
			tempMap.floor[i] = append(tempMap.floor[i], make([]bool, expandX)...)
			tempMap.seen[i] = append(tempMap.seen[i], make([]bool, expandX)...)
		} else if expandX != 0 {
			tempMap.floor[i] = append(make([]bool, -expandX), tempMap.floor[i]...)
			tempMap.seen[i] = append(make([]bool, -expandX), tempMap.seen[i]...)
		}
	}
	tempMap.width += int(math.Abs(float64(expandX)))

	// Expands along Y axis.
	addedHeight := int(math.Abs(float64(expandY)))
	if expandY > 0 {
		addition := make([][]bool, addedHeight)
		additionSeen := make([][]bool, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
			additionSeen[j] = make([]bool, tempMap.width)
		}
		tempMap.floor = append(tempMap.floor, addition...)
		tempMap.seen = append(tempMap.seen, additionSeen...)
	} else if expandY != 0 {
		addition := make([][]bool, addedHeight)
		additionSeen := make([][]bool, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
			additionSeen[j] = make([]bool, tempMap.width)
		}
		tempMap.floor = append(addition, tempMap.floor...)
		tempMap.seen = append(additionSeen, tempMap.seen...)

	}
	// fmt.Println(tempMap.floor)

	tempMap.height += addedHeight

	return
}

// Prints out the state of the map in a nice way.
func (this *Map) Print(path [][]bool) {
	for y := 0; y < this.height; y++ {
		for x := 0; x < this.width; x++ {
			robotX, robotY := this.PointToBitmapCoordinate(this.robot.x, this.robot.y)
			if x == robotX && y == robotY {
				fmt.Print("* ")
			} else {
				if this.floor[y][x] {
					fmt.Print("X ")
				} else {
					if path != nil && path[y][x] {
						fmt.Print(". ")
					} else {
						fmt.Print("  ")
					}
				}
			}
		}
		fmt.Println("")
	}
}

// Converts a point (float64) to a co-ordinate (int) in the bitmap.
func (this *Map) PointToBitmapCoordinate(x, y float64) (x1, y2 int) {
	return int(x), int(y)
}

// Adds a wall based on a line starting from the robot.
func (this *Map) AddWallByLine(degree, distance float64) {
	x, y := this.LineToBitmapCoordinate(degree, distance)
	this.AddWall(x, y)
	// Mark all blocks the line travels through as seen.
	for dist := 0; dist < int(distance); dist++ {
		x, y := this.LineToBitmapCoordinate(degree, float64(dist))
		println(x, y, this.seen)
		this.seen[y][x] = true
	}
}

// Takes the robots location, draws a line out from it at the given degree and returns the bitmap (rounded down) location  of the resulting point.
func (this *Map) LineToBitmapCoordinate(degree, distance float64) (x1, y1 int) {
	x, y := getOpposite(degree, distance)+this.robot.x, -getAdjacent(degree, distance)+this.robot.y
	return int(x), int(y)
}

// Draws a square around the robot. (Used While Testing Pathfinding)
func (this *Map) DrawSquare(n int) {
	for i := -n; i <= n; i++ {
		robotX, robotY := this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(i+robotX, -n+robotY)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(i+robotX, n+robotY)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(-n+robotX, i+robotY)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(n+robotX, i+robotY)
	}
}
