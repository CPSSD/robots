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
	return
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
		tempMap.floor[y+expandY][x+expandX] = true
	} else {
		tempMap.floor[y][x] = true
	}
	this = tempMap
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
	// fmt.Println(tempMap.floor, expandX, expandY)
	for i := 0; i < tempMap.height; i++ {
		if expandX > 0 {
			tempMap.floor[i] = append(tempMap.floor[i], make([]bool, expandX)...)
		} else if expandX != 0 {
			tempMap.floor[i] = append(make([]bool, -expandX), tempMap.floor[i]...)
		}
	}
	// fmt.Println(tempMap.floor)
	tempMap.width += int(math.Abs(float64(expandX)))

	addedHeight := int(math.Abs(float64(expandY)))
	// fmt.Println(tempMap.floor)
	if expandY > 0 {
		addition := make([][]bool, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
		}
		tempMap.floor = append(tempMap.floor, addition...)
	} else if expandY != 0 {
		addition := make([][]bool, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
		}
		tempMap.floor = append(addition, tempMap.floor...)

	}
	// fmt.Println(tempMap.floor)

	tempMap.height += addedHeight

	return
}

func (this *Map) Print(path [][]bool) {
	for y := 0; y < this.height; y++ {
		for x := 0; x < this.width; x++ {
			robotX, robotY := this.PointToBitmapCoordinate(this.robot.x, this.robot.y)
			if x == robotX && y == robotY {
				fmt.Print("*  ")
			} else {
				if this.floor[y][x] {
					fmt.Print("[] ")
				} else {
					if path != nil && path[y][x] {
						fmt.Print(".. ")
					} else {
						fmt.Print("   ")
					}
				}
			}
		}
		fmt.Println("")
	}
}

func (this *Map) PointToBitmapCoordinate(x, y float64) (x1, y2 int) {
	return int(x), int(y)
}

// Takes the robots location, draws a line out from it at the given degree and returns the bitmap (rounded down) location  of the resulting point.
func (this *Map) LineToBitmapCoordinate(degree, distance float64) (x1, y1 int) {
	x, y := getOpposite(degree, distance)+this.robot.x, -getAdjacent(degree, distance)+this.robot.y
	return int(x), int(y)
}

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
