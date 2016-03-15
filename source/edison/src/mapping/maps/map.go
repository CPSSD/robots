// Handles all the mapping code.

package maps

import (
	"RobotDriverProtocol"
	"fmt"
	"math"
	"sort"
	"time"
)

// BitmapScale is the size of each bitmap segment in millimeters
const BitmapScale = 2

// Debug indicates whether to print verbose debugging output
const Debug = false

var finishedMapping = false

// RobotMap is the current environment being mapped
var RobotMap Map

// Map represents a two dimentional map of the environment we are mapping
type Map struct {
	width      int
	height     int
	lastWidth  int
	lastHeight int

	robot Robot

	floor [][]bool
	seen  [][]int
}

// MapInit initialises the map and the rdp library, and starts a scan.
func MapInit() {
	fmt.Println("[Initialising Map]")
	RobotMap = CreateMap()
	fmt.Println("[Map Created]")
	fmt.Println("[Initialising RDP]")
	RDPInit()
	fmt.Println("[RDP Link Ready]")
	fmt.Println(RobotMap)
	RobotDriverProtocol.Scan()
}

// GetSeenMap returns the seen map.
func (this *Map) GetSeenMap() [][]int {
	return this.seen
}

// GetBitmap returns the current bitmap and a boolean denoting if the map is complete.
func (this *Map) GetBitmap() ([][]bool, bool) {
	return this.floor, finishedMapping
}

// GetRobot returns the maps robot.
func (this *Map) GetRobot() *Robot {
	return &this.robot
}

// CreateMap creates an empty 1x1 map (this single point will be the robots starting position.)
func CreateMap() (createdMap Map) {
	createdMap = Map{width: 1, height: 1, robot: Robot{0.5, 0.5, 0}}
	createdMap.floor = append(createdMap.floor, []bool{false})
	createdMap.seen = append(createdMap.seen, []int{1})
	return
}

// MoveRobotAlongPath moves the robot along the given path.
// Set "stopBeforePoint" to true if you dont want to stop before entering the point given.
// Used when following a path into unseen areas to prevent crashes.
func (this *Map) MoveRobotAlongPath(path [][]bool, stopBeforePoint bool) {
	prevX, prevY := -1, -1
	movesLeft := true
	nextX, nextY := 0, 0
	for movesLeft {
		nextX, nextY, movesLeft = this.getNextMove(int(this.robot.x), int(this.robot.y), prevX, prevY, path)
		if stopBeforePoint {
			_, _, moreMoves := this.getNextMove(nextX, nextY, int(this.robot.x), int(this.robot.y), path)
			if !moreMoves {
				fmt.Println("Made it to last point before goal.")
				return
			}
		}
		prevX, prevY = int(this.robot.x), int(this.robot.y)
		//this.robot.MoveToPoint(nextX, nextY, true)
		degree, magnitude := getHorizontalLine(prevX, prevY, nextX, nextY)

		RobotDriverProtocol.Move(uint16(degree), uint32(magnitude))

		tick := 0
		waiting := true

		// While the robot hasn't moved...
		for waiting && int(this.robot.x) == prevX && int(this.robot.y) == prevY {
			// Wait for 5 seconds, then exit and try again.
			if tick >= 50 {
				waiting = false
			}
			fmt.Println("Waiting for response from Arduino. [Sleeping for 3 seconds]")
			time.Sleep(100 * time.Millisecond)
			tick++
		}
	}

	fmt.Println("Finished Moving along path. [Sending Scan Request]")
	RobotDriverProtocol.Scan()
}

func getHorizontalLine(x1, y1, x2, y2 int) (degree, magnitude float64) {
	if x1+1 == x2 {
		return 90, BitmapScale
	}
	if x1-1 == x2 {
		return 270, BitmapScale
	}
	if y1-1 == y2 {
		return 0, BitmapScale
	}
	if y1+1 == y2 {
		return 180, BitmapScale
	}
	return 0, 0
}

// MoveRobotAlongLine moves the robot to the end point of the line.
func (this *Map) MoveRobotAlongLine(degree, magnitude float64) {
	this.robot.MoveAlongLine(degree, scale(magnitude))
}

// MoveRobotToPoint moves the robot to a point.
func (this *Map) MoveRobotToPoint(x, y int) {
	this.robot.MoveToPoint(x, y, true)
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

// AddWall adds a wall in position (x, y) of the map. Resized represents if the co-ordinates have been modified due to the BitmapScale const or not. Expands if neccesary
func (this *Map) AddWall(x, y int, resized bool) {
	if !resized {
		x, y = ScaleCoordinate(float64(x), float64(y))
	}
	tempMap := this

	if !this.pointInMap(x, y) {
		expandX, expandY := 0, 0
		tempMap, expandX, expandY = this.expandMap(x, y)
		if Debug {
			fmt.Println("expandX:", expandX, "expandY:", expandY)
		}
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

		//		expandX, expandY = ScaleCoordinate(expandX, expandY)
		//		x, y = ScaleCoordinate(x, y)
		tempMap.robot.x += float64(expandX)
		tempMap.robot.y += float64(expandY)
		tempMap.seen[y+expandY][x+expandX] = -1
		tempMap.floor[y+expandY][x+expandX] = true
	} else {
		if Debug {
			fmt.Println("Size of Seen Array:", len(tempMap.seen), "*", len(tempMap.seen[0]))
			fmt.Println("Entering at location::", x, y)
		}
		tempMap.seen[y][x] = -1
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
	} else if x >= 0 && x < this.width {
		expandX = 0
	}

	if y >= this.height {
		expandY = y + 1 - this.height
	} else if y >= 0 && y < this.height {
		expandY = 0
	}

	if Debug {
		fmt.Println("[expandMap(x, y)]: expandY:", expandY)
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
			tempMap.seen[i] = append(tempMap.seen[i], make([]int, expandX)...)
		} else if expandX != 0 {
			tempMap.floor[i] = append(make([]bool, -expandX), tempMap.floor[i]...)
			tempMap.seen[i] = append(make([]int, -expandX), tempMap.seen[i]...)
		}
	}
	tempMap.width += int(math.Abs(float64(expandX)))

	// Expands along Y axis.
	addedHeight := int(math.Abs(float64(expandY)))
	if expandY > 0 {
		addition := make([][]bool, addedHeight)
		additionSeen := make([][]int, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
			additionSeen[j] = make([]int, tempMap.width)
		}
		tempMap.floor = append(tempMap.floor, addition...)
		tempMap.seen = append(tempMap.seen, additionSeen...)
	} else if expandY != 0 {
		addition := make([][]bool, addedHeight)
		additionSeen := make([][]int, addedHeight)
		for j := 0; j < addedHeight; j++ {
			addition[j] = make([]bool, tempMap.width)
			additionSeen[j] = make([]int, tempMap.width)
		}
		tempMap.floor = append(addition, tempMap.floor...)
		tempMap.seen = append(additionSeen, tempMap.seen...)

	}
	// fmt.Println(tempMap.floor)

	tempMap.height += addedHeight

	return
}

// Print ouputs the state of the map in a nice way.
func (this *Map) Print(path [][]bool) {
	for y := 0; y < this.height; y++ {
		for x := 0; x < this.width; x++ {
			robotX, robotY := int(this.robot.x), int(this.robot.y)
			if x == robotX && y == robotY {
				fmt.Print("* ")
			} else {
				if this.floor[y][x] {
					fmt.Print("X ")
				} else {
					if this.seen[y][x] == 0 {
						fmt.Print(this.seen[y][x], " ")
					} else {
						fmt.Print("  ")
					}
				}
			}
		}
		fmt.Println("")
	}
	fmt.Println("----------------------------------------")
}

func scale(x float64) float64 {
	if x != 0 {
		return float64(x / BitmapScale)
	}
	return 0
}

// ScaleCoordinate transforms a raw reading from the laser to a position on the bitmap
func ScaleCoordinate(x, y float64) (int, int) {
	return int(scale(x)), int(scale(y))
}

// LineToBitmapCoordinate takes the robots location, draws a line out from it at the given degree and returns the bitmap (rounded down) location  of the resulting point.
func (this *Map) LineToBitmapCoordinate(degree, distance float64) (int, int) {
	x, y := getOpposite(degree, distance)+this.robot.x, -getAdjacent(degree, distance)+this.robot.y
	return int(x), int(y)
}

// AddWallByLine adds a wall based on a line starting from the robot.
func (this *Map) AddWallByLine(degree, distance float64) {
	distance = scale(distance)
	x, y := this.LineToBitmapCoordinate(degree, distance)
	if Debug {
		fmt.Println("Adding Wall @", x, y)
	}
	this.AddWall(x, y, true)
	this.MarkLineAsSeen(degree, distance)
}

// MarkLineAsSeen marks anything the line passes through as "seen".
func (this *Map) MarkLineAsSeen(degree, distance float64) {
	if Debug {
		fmt.Println(RobotMap)
		RobotMap.Print(nil)
		fmt.Println(degree, distance)
	}
	for dist := 0; dist < int(distance); dist++ {
		if Debug {
			fmt.Println(degree, dist)
		}
		x, y := this.LineToBitmapCoordinate(degree, float64(dist))
		if this.pointInMap(x, y) {
			if Debug {
				fmt.Println(x, y)
			}
			if this.seen[y][x] == 0 {
				this.seen[y][x] = 1
			}
		}
	}
}

// Counts the ammount of seen tiles (that are just empty space) surrounding the current tile (horizontaly and vertically)
func (this *Map) getAdjacentSeenTilesCount(x, y int) (count int) {
	count++
	if y+1 < len(this.floor) {
		if this.seen[y+1][x] == 1 {
			count++
		}
	}
	if x+1 < len(this.floor[y]) {
		if this.seen[y][x+1] == 1 {
			count++
		}
	}
	if y-1 >= 0 {
		if this.seen[y-1][x] == 1 {
			count++
		}
	}
	if x-1 >= 0 {
		if this.seen[y][x-1] == 1 {
			count++
		}
	}
	return
}

// ContinueToNextArea figures out where to scan next and goes there.
func (this *Map) ContinueToNextArea() {
	// Adds all potential tiles to a queue
	var list []Node
	for y := 0; y < len(this.floor); y++ {
		for x := 0; x < len(this.floor[y]); x++ {
			//			x, y = ScaleCoordinate(x, y)
			if this.seen[y][x] == 0 {
				if this.getAdjacentSeenTilesCount(x, y) >= 1 {
					list = append(list, Node{x: x, y: y})
				}
			}
		}
	}

	// Cycles through getting the distance to each tile.
	for i := 0; i < len(list); i++ {
		list[i].getDistanceToGoal((int)(this.robot.x), (int)(this.robot.y))
	}

	// Sorts list by Distance to Robot.
	list = sortNodeList(list)

	// Prints out list.
	if Debug {
		fmt.Println("Sorted: ")
		for i := 0; i < len(list); i++ {
			fmt.Println(list[i].x, list[i].y, list[i].distanceToGoal)
		}
	}

	// Iterates over the list attempting to find a valid route to take.
	var possible bool
	//var path [][]bool
	for i := 0; i < len(list); i++ {
		path, possible := GetRoute(*this, list[i].x, list[i].y)
		if possible {
			this.MoveRobotAlongPath(path, true)
			return
		}
		print("No valid path to node", i, "checking next node.")
	}
	if !possible {
		finishedMapping = true
	}
}

// SortNodes implements the Sort interface for the Node type
type SortNodes []Node

func (slice SortNodes) Len() int           { return len(slice) }
func (slice SortNodes) Swap(i, j int)      { slice[i], slice[j] = slice[j], slice[i] }
func (slice SortNodes) Less(i, j int) bool { return slice[i].distanceToGoal < slice[j].distanceToGoal }

func sortNodeList(list []Node) []Node {
	sort.Sort(SortNodes(list))
	return list
}
