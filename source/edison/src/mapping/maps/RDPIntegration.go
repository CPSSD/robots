package maps

import (
	"RobotDriverProtocol"
	"fmt"
)

var lastAction string
var currentID int

// RDPInit Initialises the RDP stuff.
func RDPInit() {
	fmt.Println("* Registering Response Handler")
	RobotDriverProtocol.RegisterResponseHandler(RDPConnector)
	RobotDriverProtocol.Init()
}

// RDPConnector handles the incoming data.
func RDPConnector(data interface{}) {
	switch response := data.(type) {
	case RobotDriverProtocol.MoveResponse:
		moveResponse(response)
	case RobotDriverProtocol.RotateResponse:
		rotateResponse(response)
	case RobotDriverProtocol.ScanResponse:
		scanResponse(response)
	case RobotDriverProtocol.StopResponse:
		stopResponse(response)
	case RobotDriverProtocol.CompassResponse:
		compassResponse(response)
	}
}

func compassResponse(response RobotDriverProtocol.CompassResponse) {
	fmt.Println("[Compass Response] Angle: ", response.Angle)
	UpdateCompassHeading(int(response.Angle))
}

func moveResponse(response RobotDriverProtocol.MoveResponse) {
	fmt.Print("[Move Response] Angle:", response.Angle, " // Magnitude:", response.Magnitude)

	if followingPath {
		lastX := RobotMap.GetRobot().GetX()
		lastY := RobotMap.GetRobot().GetY()
		RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
		fmt.Println("[RDPProtocol] Taking Next Step... | Current Location: (", RobotMap.GetRobot().GetX(), ", ", RobotMap.GetRobot().GetY(), ")")
		RobotMap.Print(path)
		RobotMap.TakeNextStep(int(lastX), int(lastY))
	} else {
		// Updates robots location to the responses location.
		RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
		lastAction = "Move"
		currentID = -1
		RobotDriverProtocol.Scan()
	}
}

func scanResponse(response RobotDriverProtocol.ScanResponse) {
	fmt.Println("[Scan Response] Degree: ", response.Degree, " // Distance: ", response.Distance)

	// Add a wall at the specific location.
	// When last response, find next location to move to in map.go
	scanBuffer = append(scanBuffer, response)

	if response.Last {
		if firstScan {
			firstScan = false
			RobotMap.addBufferToMap()
		} else {
			fmt.Println("Robot should be here at this location: (", RobotMap.GetRobot().GetX(), ", ", RobotMap.GetRobot().GetY(), ")")
			x, y, rotation := RobotMap.FindLocation()
			RobotMap.GetRobot().MoveToPoint(x, y, true)
			RobotMap.GetRobot().Rotate(float64(rotation))
			RobotMap.addBufferToMap()
		}

		lastAction = "Scan"
		currentID = -1

		RobotMap.Print(nil)
		RobotMap.ContinueToNextArea()
	}
}

func rotateResponse(response RobotDriverProtocol.RotateResponse) {
	fmt.Print("[Rotate Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "}")

	// Changes the rotation of the robot
	RobotMap.GetRobot().Rotate(float64(response.Angle))
	lastAction = "Rotate"
	currentID = -1
}

func stopResponse(response RobotDriverProtocol.StopResponse) {
	fmt.Print("[Stop Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "}")

	// Updates robots location to the response location.
	// Check reason, do something based on reason.
	RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
	lastAction = "Stop"
	currentID = -1

	RobotDriverProtocol.Scan()

}
