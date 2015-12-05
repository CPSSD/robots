// Handles all the robot specific code. (Currently just getters)

package maps

import "fmt";

type Robot struct {
	x        float64
	y        float64
	rotation int
}

func (this Robot) GetX() float64 {
	return this.x
}

func (this Robot) GetY() float64 {
	return this.y
}

func (this Robot) GetRotation() int {
	return this.rotation
}

func (this *Robot) MoveToPoint(x, y int, resized bool) {
	if !resized {
	//	x, y = RobotMap.PointToBitmapCoordinate(float64(x), float64(y))
	}
	this.x = float64(x)
	this.y = float64(y)
}

func (this *Robot) MoveAlongLine(degree, magnitude float64){
	x, y := RobotMap.LineToBitmapCoordinate(degree, magnitude)
	this.MoveToPoint(x, y, true)
	fmt.Println("Robots Current Data:", *this)
}
