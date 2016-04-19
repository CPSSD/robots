// Handles all the mapping code.

package maps

import (
	"RobotDriverProtocol"
	"encoding/csv"
	"fmt"
	"math"
	"os"
	"sort"
	"strconv"
)

// Debug indicates whether to print verbose debugging output
const Debug = false

var BitmapScale int
var scanBuffer []RobotDriverProtocol.ScanResponse
var finishedMapping = false

// RobotMap is the current environment being mapped
var firstScan = true
var checkLocation = false
var followingPath = false

var pathIndex = 0

// RobotMap is a map of the current room being mapped
var RobotMap Map
var path [][]bool
var lineMap []Line

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
func MapInit(bitmapScale int) {
	BitmapScale = bitmapScale
	fmt.Println("[Initialising Map]")

	RobotMap = CreateMap()
	fmt.Println("[Map Created]")
	fmt.Println("[Initialising RDP]")

	RDPInit()
	fmt.Println("[RDP Link Ready]")

	for i := 0; i < 360; i++ {
		if i >= 250 {
			RobotMap.AddWallByLine(float64(i), 80)
		} else {
			RobotMap.AddWallByLine(float64(i), 200)
		}
	}
	RobotMap.AddWallByLine(270, 200)
	RobotMap.GetRobot().Rotate(0);
	RobotMap.ContinueToNextArea()
	RobotMap.Print(path)

	scanBuffer = make([]RobotDriverProtocol.ScanResponse, 0)

	//defer RobotMap.Print(nil)

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

// FollowingPath Returns the followingPath boolean
func FollowingPath() bool {
	return followingPath
}

// CreateMap creates an empty 1x1 map (this single point will be the robots starting position.)
func CreateMap() (createdMap Map) {
	createdMap = Map{width: 1, height: 1, robot: Robot{0, 0, 0}}
	createdMap.floor = append(createdMap.floor, []bool{false})
	createdMap.seen = append(createdMap.seen, []int{1})
	return
}

//SaveMap saves a Map to a file
func (this *Map) SaveMap(fileName string) {
	mapCSV, err := os.Create(fileName)
	if err != nil {
		fmt.Println("Error saving map: ", err)
		return
	}

	writer := csv.NewWriter(mapCSV)

	writer.Write([]string{strconv.Itoa(this.height), strconv.Itoa(this.width)})

	for i := 0; i < this.height; i++ {
		row := make([]string, this.width)
		for j := 0; j < this.width; j++ {

			row[j] = strconv.FormatBool(this.floor[i][j])
		}
		writer.Write(row)
	}

	writer.Flush()
	mapCSV.Close()
}

//LoadMap reads a Map from a file
func (this *Map) LoadMap(fileName string) {
	mapCSV, err := os.Open(fileName)
	if err != nil {
		fmt.Println("Error loading map: ", err)
	}
	defer mapCSV.Close()

	reader := csv.NewReader(mapCSV)

	record, err := reader.Read()
	this.height, _ = strconv.Atoi(record[0])
	this.width, _ = strconv.Atoi(record[1])

	this.floor = make([][]bool, this.height)

	for i := 0; i < this.height; i++ {
		this.floor[i] = make([]bool, this.width)
		record, _ := reader.Read()
		for j := 0; j < this.width; j++ {
			this.floor[i][j], _ = strconv.ParseBool(record[j])
		}
	}
}

// Creates a fragment of a map, containing all lines from the buffer.
func createMapFragment(buffer []RobotDriverProtocol.ScanResponse, rotation int) Map {
	fragment := CreateMap()
	for _, line := range buffer {
		fragment.AddWallByLine(float64((int(line.Degree)+rotation)%360), float64(line.Distance))
	}
	fmt.Println("[", rotation, "] Robot Location: (", fragment.GetRobot().GetX(), ", ", fragment.GetRobot().GetY(), ")")
	return fragment
}

// Adds the last buffer of scan responses to the map, then clears the buffer.
func (this *Map) addBufferToMap() {
	for _, response := range scanBuffer {
		RobotMap.AddWallByLine(float64(response.Degree)+float64(this.GetRobot().GetRotation()), float64(response.Distance))
	}
	scanBuffer = make([]RobotDriverProtocol.ScanResponse, 0)
}

// FindLocation finds and returns the location of the robot based on the current scan information and the previous map information
func (this *Map) FindLocation() (int, int, float64) {
	rotationAmmount := 30
	rotationJump := 1
	x, y := int(this.GetRobot().GetX()), int(this.GetRobot().GetY())
	var count = -999
	var angle = int(this.GetRobot().GetRotation())
	var currentRotation = int(this.GetRobot().GetRotation())
	for i := -rotationAmmount; i <= rotationAmmount; i += rotationJump {
		currentRotation = i + int(this.GetRobot().GetRotation())
		tempX, tempY, tempCount := this.findLocation(createMapFragment(scanBuffer, currentRotation))
		if tempCount > count {
			x = tempX
			y = tempY
			count = tempCount
			angle = i
		}
	}
	fmt.Println("Most Likely Position after rotation is: (", x, ", ", y, ")")
	return x, y, float64(angle)
}

func (this *Map) findLocation(fragment Map) (int, int, int) {
	fmt.Println("Attempting to find location...")
	mX, mY, mCount := int(this.GetRobot().GetX()), int(this.GetRobot().GetY()), 0

	fmt.Println("Robots Assumed Location: (", mX, ",", mY, ")")

	for i := 0; i < this.width; i++ {
		for j := 0; j < this.height; j++ {
			if i >= 0 && j >= 0 && i < this.width && j < this.height {
				count, _, _ := this.probabilityAtLocation(fragment, int(i), int(j))
				if count != 0 {
					// fmt.Print(count, " ")
					if count > mCount {
						mX = i - (fragment.width / 2) + int(fragment.GetRobot().GetX())
						mY = j - (fragment.height / 2) + int(fragment.GetRobot().GetY())
						mCount = count
					}
				} else {
					// fmt.Print("  ")
				}
			}
		}
		// fmt.Println("")
	}

	fmt.Println("Most Likely Position: (", mX, ", ", mY, "): ", mCount)
	return mX, mY, mCount
}

func (this *Map) probabilityAtLocation(fragment Map, x int, y int) (int, int, int) {
	count := 0
	height := len(fragment.floor)
	for yOffset := 0; yOffset < height; yOffset++ {
		width := len(fragment.floor[yOffset])
		for xOffset := 0; xOffset < width; xOffset++ {
			checkX := x + xOffset - width/2
			checkY := y + yOffset - height/2
			if checkX >= 0 && checkY >= 0 && checkX < this.width && checkY < this.height {
				if fragment.floor[yOffset][xOffset] == true && fragment.floor[yOffset][xOffset] == RobotMap.floor[checkY][checkX] {
					count++
				}
			}
		}
	}
	return count, x, y
}

func (this *Map) TakeNextStep(lastX int, lastY int) {
	if len(path) != 0 {
		robotPoint := Point{int(this.GetRobot().GetX()), int(this.GetRobot().GetY())}
		line, movesLeft := this.getNextMove(int(this.GetRobot().GetX()), int(this.GetRobot().GetY()), lastX, lastY, path)

		fmt.Println("currentLocation: ", robotPoint)
		// If you are 1 move away from end point
		if !movesLeft {
			fmt.Println("No more steps to take...")
			fmt.Println("... ", line)
			path = make([][]bool, 0)
			lineMap = make([]Line, 0)
			followingPath = false

			RobotDriverProtocol.Scan()
			return
		}

		fmt.Println("[TakeNextStep]: (", line, ") | From Location: (", robotPoint, ")")

		degree := line.getAngleFrom(robotPoint)
		magnitude := line.getLength()

		fmt.Println("[TakeNextStep]: Required Move: ", degree, " -> ", magnitude)

		RobotDriverProtocol.Move(uint16(degree), uint32(magnitude))
	} else {
		fmt.Println("No more steps to take...")
		path = make([][]bool, 0)
		followingPath = false

		RobotDriverProtocol.Scan()
	}
}

// MoveRobotAlongPath moves the robot along the given path.
// Set "stopBeforePoint" to true if you dont want to stop before entering the point given.
// Used when following a path into unseen areas to prevent crashes.
func (this *Map) MoveRobotAlongPath(newPath [][]bool, stopBeforePoint bool) {
	path = newPath
	followingPath = true
	this.TakeNextStep(-1, -1)
	fmt.Println("Queued a path for movement...")
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
func (this *Map) getNextMove(x, y, prevX, prevY int, path [][]bool) (line Line, possibleMove bool) {
	lines := BitmapToVector(path)
	fmt.Println("Line Map: ", lines)
	fmt.Println("Index: ", pathIndex, " | Size: ", len(lines))

	robotPoint := Point{x, y}
	prevRobotPoint := Point{prevX, prevY}

	for _, line := range lines {
		if Debug {
			fmt.Println("Did ", line, " get past here?")
			fmt.Println("Previous Loc: ", prevRobotPoint)
		}
		if !line.pointOnLine(prevRobotPoint) {
			if Debug {
				fmt.Println("\t", line, " contains ", robotPoint, " => ", line.pointOnLine(robotPoint))
			}
			if line.pointOnLine(robotPoint) {
				return line, true
			}
		}
	}

	return Line{}, false
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
	if !Debug {
		//return
	}
	for y := 0; y < this.height; y++ {
		for x := 0; x < this.width; x++ {
			robotX, robotY := int(this.robot.x), int(this.robot.y)
			if x == robotX && y == robotY {
				fmt.Print("* ")
			} else {
				if path != nil && path[y][x] {
					fmt.Print("~ ")
				} else if this.floor[y][x] {
					fmt.Print("X ")
				} else {
					if this.seen[y][x] == 0 {
						fmt.Print("# ")
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
		return float64(x / float64(BitmapScale))
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
	count = 0
	for yOffset := -1; yOffset <= 1; yOffset++ {
		for xOffset := -1; xOffset <= 1; xOffset++ {
			if !(yOffset == 0 && xOffset == 0) {
				if y+yOffset >= 0 && x+xOffset >= 0 && y+yOffset < len(this.floor) && x+xOffset < len(this.floor[0]) {
					if this.seen[y+yOffset][x+xOffset] == 1 {
						count++
					}
				}
			}
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
			this.Print(path)
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
