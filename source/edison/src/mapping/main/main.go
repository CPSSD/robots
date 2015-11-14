// This entire file will be removed at some point in the future.
// Its just here to test out the library.
package main

import (
	"fmt"
	"mapping/maps"
	"mapping/pathfinding"
)

func main() {
	pathfindingTest()
}

func pathfindingTest(){
	// Creating map to test in.
	RobotMap := maps.CreateMap()
	RobotMap.DrawSquare(4)
	RobotMap.AddWall(4, 5)
	RobotMap.AddWall(4, 6)
	RobotMap.AddWall(4, 3)
	RobotMap.AddWall(4, 4)
	RobotMap.AddWall(3, 4)
	RobotMap.AddWall(1, 4)
	RobotMap.AddWall(4, 1)
	RobotMap.AddWall(4, 2)
	RobotMap.MoveRobotTo(360-90, 2)
	RobotMap.MoveRobotTo(0, 1)

	// Pathfind to point (1, 7)
	path, success := pathfinding.GetRoute(RobotMap, 6, 1)

	// Prints out the map with the route denoted by .'s
	if success {
		RobotMap.Print(path)
	} else {
		RobotMap.Print(nil)
		fmt.Println("No valid path found")
	}
}
