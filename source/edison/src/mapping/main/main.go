// This entire file will be removed at some point in the future.
// Its just here to test out the library.
package main

import (
	"fmt"
	"mapping/maps"
)

func main() {
	exampleRobot()
}

func exampleRobot() {
	RobotMap := maps.CreateMap()
	RobotMap.Print()

	fmt.Println()
	// This section just draws a rectangle around the robots current position (changes whenever map expands) using points.
	n := 40
	for i := -n; i <= n; i++ {
		robotX, robotY := RobotMap.PointToBitmapCoordinate(RobotMap.GetRobot().GetX(), RobotMap.GetRobot().GetY())
		RobotMap.AddWall(i+robotX, -n+robotY)

		robotX, robotY = RobotMap.PointToBitmapCoordinate(RobotMap.GetRobot().GetX(), RobotMap.GetRobot().GetY())
		RobotMap.AddWall(i+robotX, n+robotY)

		robotX, robotY = RobotMap.PointToBitmapCoordinate(RobotMap.GetRobot().GetX(), RobotMap.GetRobot().GetY())
		RobotMap.AddWall(-n+robotX, i+robotY)

		robotX, robotY = RobotMap.PointToBitmapCoordinate(RobotMap.GetRobot().GetX(), RobotMap.GetRobot().GetY())
		RobotMap.AddWall(n+robotX, i+robotY)

	}
	fmt.Println("Drew a Box thats", n, "squared, around the Robot.")
//	RobotMap.Print()
//	fmt.Println()

	// This section uses the robots location, degree of the "laser", and distance returned by the "laser" to draw a circle.
	for i := 0; i <= 360; i += 1 {
		x, y := RobotMap.LineToBitmapCoordinate(float64(i), 30)
		RobotMap.AddWall(x, y)
	}

	RobotMap.MoveRobotTo(0, 25)

	for i := 0; i <= 360; i += 1 {
		x, y := RobotMap.LineToBitmapCoordinate(float64(i), 15)
		RobotMap.AddWall(x, y)
	}

	fmt.Println("Drew circle with", 4.4, "radius around the Robot.")
	RobotMap.Print()
	fmt.Println()
}
