// Handles all the robot specific code. (Currently just getters)

package maps

import "fmt"

type Robot struct {
	x        float64
	y        float64
	rotation float64
}

// GetX returns the robots x value.
func (this Robot) GetX() float64 {
	return this.x
}

// GetY returns the robots y value.
func (this Robot) GetY() float64 {
	return this.y
}

// GetRotation returns the robots rotation.
func (this Robot) GetRotation() float64 {
	return this.rotation
}

// Rotate 's the robot.
func (this *Robot) Rotate(degree float64) {
	this.rotation += degree
	for this.rotation > 360 {
		this.rotation -= 360
	}
}

// MoveToPoint moves the robot to the given point.
func (this *Robot) MoveToPoint(x, y int, resized bool) {
	if !resized {
		//	x, y = RobotMap.PointToBitmapCoordinate(float64(x), float64(y))
	}
	this.x = float64(x)
	this.y = float64(y)
}

// MoveAlongLine moves the robot along a line.
func (this *Robot) MoveAlongLine(degree, magnitude float64) {
	x, y := RobotMap.LineToBitmapCoordinate((degree+this.rotation)%360, magnitude)
	this.MoveToPoint(x, y, true)
	if Debug {
		fmt.Println("Robots Current Data:", *this)
	}
}
