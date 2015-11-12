// Handles all the robot specific code. (Currently just getters)

package maps

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
