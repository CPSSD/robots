// Handles all the mapping code.

package maps

import "fmt"
import "math"

const SIZE = 25 // Millimeters Per Bitmap Segment

var RobotMap Map

type Map struct {
	width       int
	height      int
	last_width  int
	last_height int

	robot Robot

	floor [][]bool
	seen  [][]int
}

func MapInit() {
	RobotMap = CreateMap()
	RDPInit()
	fmt.Println(RobotMap);
}

func (this *Map) GetSeenMap() [][] int {
	return this.seen
}

func (this *Map) GetBitmap() [][]bool {
	return this.floor
}

// Returns the maps robot.
func (this *Map) GetRobot() *Robot {
	return &this.robot
}

// Creates an empty 1x1 (this single point will be the robots starting position.)
func CreateMap() (createdMap Map) {
	createdMap = Map{width: 1, height: 1, robot: Robot{0.5, 0.5, 0}}
	createdMap.floor = append(createdMap.floor, []bool{false})
	createdMap.seen = append(createdMap.seen, []int{1})
	return
}

// Moves the robot along the path.
func (this *Map) MoveRobotAlongPath(path [][]bool, stopBeforePoint bool) {
	prevX, prevY := -1, -1
	possibleMove := true
	nextX, nextY := 0, 0
	for possibleMove {
		nextX, nextY, possibleMove = this.getNextMove(int(this.robot.x), int(this.robot.y), prevX, prevY, path)
		if stopBeforePoint {
			_, _, moreMoves := this.getNextMove(nextX, nextY, int(this.robot.x), int(this.robot.y), path)
			if !moreMoves {
				fmt.Println("Made it to last point before goal.")
				return
			}
		}
		prevX, prevY = int(this.robot.x), int(this.robot.y)
		this.robot.MoveToPoint(nextX, nextY, true)
	}
	fmt.Println("Finished Moving along path.")
}

func (this *Map) MoveRobotAlongLine(degree, magnitude float64){
	this.robot.MoveAlongLine(degree, scale(magnitude))
}

func (this *Map) MoveRobotToPoint(x, y int){
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

// Adds a wall in position (x, y) of the map. Resized represents if the co-ordinates have been modified due to the SIZE const or not. Expands if neccesary
func (this *Map) AddWall(x, y int, resized bool) {
	if !resized {
		x, y = ScaleCoordinate(float64(x), float64(y))
	}
	tempMap := this
	fmt.Println("*** Upon Entering AddWall: ", x, y)
	fmt.Println("*** After Conversion In AddWall: ", x, y)

	fmt.Println("[AddWall(x, y)]: x:",x,"y:",y)

	if !this.pointInMap(x, y) {
		expandX, expandY := 0, 0
		tempMap, expandX, expandY = this.expandMap(x, y)
		fmt.Println("expandX:", expandX, "expandY:", expandY)
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
		fmt.Println("Size of Seen Array:", len(tempMap.seen), "*", len(tempMap.seen[0]))
		fmt.Println("Entering at location::", x, y)
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

		fmt.Println("[expandMap(x, y)]: expandY:", expandY)
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
					if(this.seen[y][x] == 0){
						fmt.Print(this.seen[y][x], " ")
					} else {
						fmt.Print("  ")
					}
				}
			}
		}
		fmt.Println("")
	}
	fmt.Println("--------");
}

// Converts a point (float64) to a co-ordinate (int) in the bitmap.
func (this *Map) PointToBitmapCoordinate(x, y float64) (x1, y2 int) {
	return int(x), int(y)
}

func scale(x float64) float64 {
	if x != 0 {
		return float64(x/SIZE)
	}
	return 0
}

func ScaleCoordinate(x, y float64) (x1, y1 int) {
	return int(scale(x)), int(scale(y))
}

// Takes the robots location, draws a line out from it at the given degree and returns the bitmap (rounded down) location  of the resulting point.
func (this *Map) LineToBitmapCoordinate(degree, distance float64) (x1, y1 int) {
	x, y := getOpposite(degree, distance)+this.robot.x, -getAdjacent(degree, distance)+this.robot.y
	return int(x), int(y)
}

// Adds a wall based on a line starting from the robot.
func (this *Map) AddWallByLine(degree, distance float64) {
	distance = scale(distance)
	x, y := this.LineToBitmapCoordinate(degree, distance)
	fmt.Println("Adding Wall @", x, y)
	this.AddWall(x, y, true)
	this.MarkLineAsSeen(degree, distance)
}

// Marks anything the line passes through as "seen".
func (this *Map) MarkLineAsSeen(degree, distance float64){
	fmt.Println(RobotMap)
	RobotMap.Print(nil)
	fmt.Println(degree, distance)
	for dist := 0; dist < int(distance); dist++ {
		fmt.Println(degree, dist)
		x, y := this.LineToBitmapCoordinate(degree, float64(dist))
		if this.pointInMap(x, y) {
			fmt.Println(x, y)
			if(this.seen[y][x] == 0){
				this.seen[y][x] = 1
			}
		}
	}
}

// Draws a square around the robot. (Used While Testing Pathfinding)
func (this *Map) DrawSquare(n int) {
	for i := -n; i <= n; i++ {
		robotX, robotY := this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(i+robotX, -n+robotY, true)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(i+robotX, n+robotY, true)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(-n+robotX, i+robotY, true)

		robotX, robotY = this.PointToBitmapCoordinate(this.GetRobot().GetX(), this.GetRobot().GetY())
		this.AddWall(n+robotX, i+robotY, true)
	}
}

// Counts the ammount of seen tiles (that are just empty space) surrounding the current tile (horizontaly and vertically)
func (this *Map) getAdjacentSeenTilesCount(x, y int) (count int) {
	count = 0
	if y + 1 < len(this.floor) {
		if this.seen[y+1][x] == 1 {
			count += 1
		}
	} 
	if x + 1 < len(this.floor[y]) {
		if this.seen[y][x+1] == 1 {
			count += 1
		}
	}
	if y - 1 >= 0 {
		if this.seen[y-1][x] == 1 {
			count += 1
		}
	}
	if x - 1 >= 0 {
		if this.seen[y][x-1] == 1 {
			count += 1
		}
	}
	return
}

// Figures out where to scan next and goes there.
func (this *Map) ContinueToNextArea(){
	// Adds all potential tiles to a queue
	list := make([]Node, 0)
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
		list[i].getDistanceToGoal((int) (this.robot.x), (int) (this.robot.y))
	}

	// Sorts list by Distance to Robot.
	list = sortNodeList(list)

	// Prints out list.
	fmt.Println("Sorted: ");
	for i := 0; i < len(list); i++ {
		fmt.Println(list[i].x, list[i].y, list[i].distanceToGoal)
	}

	// Iterates over the list attempting to find a valid route to take.
	for i := 0; i < len(list); i++ {
		path, possible := GetRoute(*this, list[i].x, list[i].y)
		if possible {
			this.MoveRobotAlongPath(path, true)
			return
		} else {
			print("No valid path to node", i, "checking next node.")
		}
	}
}

// Selection sort for now.
func sortNodeList(list []Node) []Node {
	for i := 0; i < len(list); i++ {
		smallest := i
		for j := i; j < len(list); j++ {
			if list[smallest].distanceToGoal > list[j].distanceToGoal {
				smallest = j
			}
		}
		temp := list[i]
		list[i] = list[smallest]
		list[smallest] = temp
	}
	return list
}
