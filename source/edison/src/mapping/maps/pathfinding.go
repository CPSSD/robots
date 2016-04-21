package maps

import (
	"fmt"
	"math"
)

type Node struct {
	id, x, y       int
	distanceToGoal int
	moveCost       int
	parent         *Node
	solid          bool
}

var radius = 40 // Radius around robot.
// GetRoute Calls various functions below and returns the end result of the pathfinding algorithm.
func GetRoute(robotMap Map, x, y int) ([][]bool, bool) {
	if Debug {
		fmt.Println("GetRouteTo(", x, ",", y, ")")
	}
	nodeMap := robotMap.createNodeMap(radius)
	getDistanceToGoal(nodeMap, x, y)
	return pathfind(nodeMap, int(robotMap.GetRobot().GetX()), int(robotMap.GetRobot().GetY()), x, y)
}

func getOffset(x, y, distance, rotation int) (xOffset, yOffset int) {
	xOffset = int(float64(distance) * math.Cos(float64(rotation)))
	yOffset = int(float64(distance) * math.Sin(float64(rotation)))
	return
}

// Creates a Node at each point of the map.
func (this *Map) createNodeMap(radius int) (nodeMap [][]Node) {
	robotMap := this.floor
	for i := 0; i < len(robotMap); i++ {
		nodeMap = append(nodeMap, make([]Node, 0))
		for j := 0; j < len(robotMap[0]); j++ {
			nodeMap[i] = append(nodeMap[i], Node{id: (j * len(robotMap[0])) + i, x: j, y: i, solid: !isValidNode(this.floor, j, i, radius)})
		}
	}
	return
}

func isValidNode(robotMap [][]bool, x int, y int, radius int) bool {
	distanceToScale := radius / BitmapScale
	laserOffsetX, laserOffsetY := getOffset(x, y, (radius/BitmapScale)/2, int(RobotMap.GetRobot().GetRotation()))
	for i := -distanceToScale; i <= distanceToScale; i++ {
		yOffset := i - laserOffsetY
		if y+yOffset < len(robotMap) && y+yOffset >= 0 {
			for j := -distanceToScale; j <= distanceToScale; j++ {
				xOffset := j - laserOffsetX
				if x+xOffset < len(robotMap[y+yOffset]) && x+xOffset >= 0 {
					if !(xOffset == 0 && yOffset == 0) {
						if distanceToScale <= getDistanceToPoint(x, y, x+xOffset, y+yOffset) {
							if robotMap[y+yOffset][x+xOffset] {
								return false
							}
						}
					}
				}
			}
		}
	}
	return true
}

// Pathfinding Algorithm. Based on A*
func pathfind(nodeMap [][]Node, startX, startY, x, y int) ([][]bool, bool) {
	closedList := make(map[int]*Node)
	openList := make(map[int]*Node)

	closedList, openList = nodeMap[startY][startX].makeAMove(nodeMap, closedList, openList)
	finished := false
	var smallest *Node
	for !finished {
		smallest = nil

		if len(openList) == 0 {
			finished = true
			break
		}

		// Get smallest move cost in the list.
		for _, value := range openList {
			if smallest == nil || value.moveCost < smallest.moveCost {
				smallest = value
			}
		}

		// If we've reached the goal.
		if smallest.x == x && smallest.y == y {
			break
		}

		closedList, openList = smallest.makeAMove(nodeMap, closedList, openList)
	}

	if smallest == nil {
		return nil, false
	}

	return smallest.createPath(len(nodeMap[0]), len(nodeMap)), true
}

// Creates a 2D bool array (path) based on the node map.
func (this Node) createPath(mapWidth, mapHeight int) (path [][]bool) {
	for y := 0; y < mapHeight; y++ {
		path = append(path, make([]bool, mapWidth))
	}

	if &this == nil {
		return
	}

	// Add in all nodes except starting point (whos parent is nill)
	for this.parent != nil {
		path[this.y][this.x] = true
		this = *this.parent
	}
	// Add in starting point
	path[this.y][this.x] = true

	return
}

// Adds all possible moves to openList and adds current node to closedList.
func (this Node) makeAMove(nodeMap [][]Node, oldClosedList, oldOpenList map[int]*Node) (closedList, openList map[int]*Node) {
	closedList = oldClosedList
	openList = oldOpenList

	// Checks if the current node is in the openList. If yes, it gets removed and put into closedList.
	_, inList := openList[this.id]
	if inList {
		delete(openList, this.id)
	}
	closedList[this.id] = &this

	this.addToList(nodeMap, this.x+1, this.y, openList, closedList)
	this.addToList(nodeMap, this.x-1, this.y, openList, closedList)
	this.addToList(nodeMap, this.x, this.y+1, openList, closedList)
	this.addToList(nodeMap, this.x, this.y-1, openList, closedList)

	return
}

// Attempts to add the node to the open list. Returns the updated list.
func (this *Node) addToList(nodeMap [][]Node, x, y int, list map[int]*Node, closedList map[int]*Node) {
	if x >= 0 && y >= 0 && y < len(nodeMap) && x < len(nodeMap[0]) {
		if !nodeMap[y][x].solid {
			if nodeMap[y][x].moveCost != 0 {
				if nodeMap[y][x].moveCost > this.moveCost+nodeMap[y][x].distanceToGoal+10 {
					// Visited before but the travel cost to it from the previous node is cheaper than the previous visit.
					nodeMap[y][x].parent = this
					nodeMap[y][x].moveCost = this.moveCost + nodeMap[y][x].distanceToGoal + 10
				}
			} else {
				// Never visited before. Initialise everything.
				nodeMap[y][x].parent = this
				nodeMap[y][x].moveCost = this.moveCost + nodeMap[y][x].distanceToGoal + 10
				if RobotMap.seen[y][x] == 1 {
					nodeMap[y][x].moveCost += 9999
				}

				_, inList := closedList[nodeMap[y][x].id]
				if !inList {
					list[nodeMap[y][x].id] = &nodeMap[y][x]
				}
			}
		}
	}
}

// Gets distance to goal for a single node (minimum steps to target (horizontal and vertical))
func (this *Node) getDistanceToGoal(x, y int) {
	this.distanceToGoal = int(math.Abs(float64((x - this.x))) + math.Abs(float64(y-this.y)))
}

// GetDistanceToPoint Gets distance between two points
func getDistanceToPoint(x1, y1, x2, y2 int) int {
	return int(math.Abs(float64((x2 - x1))) + math.Abs(float64(y2-y1)))
}

// Gets distance to goal for all nodes (minimum steps to target (horizontal and vertical))
func getDistanceToGoal(nodeMap [][]Node, x, y int) {
	for i := 0; i < len(nodeMap); i++ {
		for j := 0; j < len(nodeMap[i]); j++ {
			nodeMap[i][j].getDistanceToGoal(x, y)
		}
	}
}
