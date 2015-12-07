// This entire file will be removed at some point in the future.
// Its just here to test out the library.
package main

import (
	"fmt"
	"mapping/maps"
)

func main() {
	maps.MapInit()
	maps.RobotMap.Print(nil)
	_, isFinished := maps.RobotMap.GetBitmap()
	fmt.Println("Mapping complete?: ", isFinished)
}

func unseenAreaTest() {
	RobotMap := maps.CreateMap()
	RobotMap.AddWall(1, 1, false)
	RobotMap.AddWall(5, 2, false)
	RobotMap.AddWall(3, 3, false)
	RobotMap.AddWall(2, 1, false)
	RobotMap.AddWall(4, 2, false)
	RobotMap.AddWall(3, 3, false)
	RobotMap.AddWall(3, 2, false)
	RobotMap.AddWall(2, 2, false)
	RobotMap.Print(nil)

	bitmap, _ := RobotMap.GetBitmap()
	fmt.Println(maps.BitmapToVector(bitmap))
}

func pathfindingTest() {
	// Creating map to test in.
	RobotMap := maps.CreateMap()
	RobotMap.DrawSquare(4)
	RobotMap.AddWall(4, 5, false)
	RobotMap.AddWall(4, 6, false)
	RobotMap.AddWall(4, 3, false)
	RobotMap.AddWall(4, 4, false)
	RobotMap.AddWall(3, 4, false)
	RobotMap.AddWall(1, 4, false)
	//	RobotMap.AddWall(4, 1)
	//	RobotMap.AddWall(4, 2)
	//	RobotMap.MoveRobotTo(360-90, 2)
	//	RobotMap.MoveRobotTo(0, 1)

	RobotMap.Print(nil)
	fmt.Println()

	// Pathfind to point (5, 6) from point (2, 3)
	path, success := maps.GetRoute(RobotMap, 5, 6)

	// Prints out the map with the route denoted by .'s
	if success {
		RobotMap.Print(path)
	} else {
		RobotMap.Print(nil)
		fmt.Println("No valid path found")
	}
	fmt.Println()

	// Moves the robot along the path. Point by Point.
	RobotMap.MoveRobotAlongPath(path, false)
	RobotMap.Print(nil)
	fmt.Println()

	// Pathfind to point (1, 1) from point (5, 6)
	path, success = maps.GetRoute(RobotMap, 1, 1)

	// Prints out the map with the route denoted by .'s
	if success {
		RobotMap.Print(path)
	} else {
		RobotMap.Print(nil)
		fmt.Println("No valid path found")
	}
	fmt.Println()

	// Prints out the map again.
	RobotMap.MoveRobotAlongPath(path, false)
	RobotMap.Print(nil)

	bitmap, _ := RobotMap.GetBitmap()
	fmt.Println(maps.BitmapToVector(bitmap))

}
