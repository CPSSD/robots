package pathfinding

import (
	"fmt"
	"mapping/maps"
	"math"
)

type Node struct {
	id, x, y       int
	distanceToGoal int
	moveCost       int
	parent         *Node
	solid          bool
}

// Calls various functions below and returns the end result of the pathfinding algorithm.
func GetRoute(robotMap maps.Map, x, y int) [][]bool {
	fmt.Println("GetRouteTo(", "(", x, ",", y, ")")
	nodeMap := createNodeMap(robotMap.GetBitmap())
	getDistanceToGoal(nodeMap, x, y)
	return pathfind(nodeMap, x, y)
}

func createNodeMap(robotMap [][]bool) (nodeMap [][]Node) {
	for i := 0; i < len(robotMap); i++ {
		nodeMap = append(nodeMap, make([]Node, 0))
		for j := 0; j < len(robotMap[0]); j++ {
			nodeMap[i] = append(nodeMap[i], Node{id: (j * len(robotMap[0])) + i, x: j, y: i, solid: robotMap[i][j]})
		}
	}
	return
}

func pathfind(nodeMap [][]Node, x, y int) [][]bool {
	closedList := make(map[int]*Node)
	openList := make(map[int]*Node)

	closedList, openList = nodeMap[3][2].makeAMove(nodeMap, closedList, openList)
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

	return smallest.createPath(len(nodeMap[0]), len(nodeMap))
}

func (this Node) createPath(mapWidth, mapHeight int) (path [][]bool) {
	for y := 0; y < mapHeight; y++ {
		path = append(path, make([]bool, mapWidth))
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

				_, inList := closedList[nodeMap[y][x].id]
				if !inList {
					list[nodeMap[y][x].id] = &nodeMap[y][x]
				}
			}
		}
	}
}

// Gets Heursitic of single node(minimal steps to target (horizontal and vertical))
func (this *Node) getDistanceToGoal(x, y int) {
	this.distanceToGoal = int(math.Abs(float64((x - this.x))) + math.Abs(float64(y-this.y)))
}

// Gets Heursitic of all nodes (minimal steps to target (horizontal and vertical))
func getDistanceToGoal(nodeMap [][]Node, x, y int) {
	for i := 0; i < len(nodeMap); i++ {
		for j := 0; j < len(nodeMap[i]); j++ {
			nodeMap[i][j].getDistanceToGoal(x, y)
		}
	}
}
