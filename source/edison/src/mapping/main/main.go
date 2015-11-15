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
	RobotMap.MoveRobotTo(360-90, 2)
	RobotMap.MoveRobotTo(0, 1)

	// Pathfind to point (5, 6) from point (2, 3)
	path, success := pathfinding.GetRoute(RobotMap, 5, 6)

	// Prints out the map with the route denoted by .'s
	if success {
	 	RobotMap.Print(path)
	} else {
		RobotMap.Print(nil)
		fmt.Println("No valid path found")
	}

	// Moves the robot along the path. Point by Point.
	RobotMap.MoveRobotAlongPath(path)
	RobotMap.Print(nil)

	// Pathfind to point (1, 1) from point (5, 6)
	path, success = pathfinding.GetRoute(RobotMap, 1, 1)

	// Prints out the map with the route denoted by .'s
	if success {
	 	RobotMap.Print(path)
	} else {
		RobotMap.Print(nil)
		fmt.Println("No valid path found")
	}

	// Prints out the map again.
	RobotMap.MoveRobotAlongPath(path)
	RobotMap.Print(nil)

}
