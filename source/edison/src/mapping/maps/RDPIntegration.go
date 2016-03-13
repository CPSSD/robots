package maps

import (
	"RobotDriverProtocol"
	"fmt"
	"server"
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
	fmt.Println()
	fmt.Println("\tData Recieved =>", data)

	switch response := data.(type) {
	case RobotDriverProtocol.MoveResponse:
		moveResponse(response)
	case RobotDriverProtocol.RotateResponse:
		rotateResponse(response)
	case RobotDriverProtocol.ScanResponse:
		scanResponse(response)
	case RobotDriverProtocol.StopResponse:
		stopResponse(response)
	}

	server.ResponseHandler(data)
}

func moveResponse(response RobotDriverProtocol.MoveResponse) {
	fmt.Print("[Move Response] Angle:", response.Angle, " // Magnitude:", response.Magnitude)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "}")

	// Updates robots location to the responses location.
	RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
	lastAction = "Move"
	currentID = -1
}

func scanResponse(response RobotDriverProtocol.ScanResponse) {
	fmt.Print("[Scan Response] Degree: ", response.Degree, " // Distance: ", response.Distance)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "}")

	// Add a wall at the specific location.
	// When last response, find next location to move to in map.go
	RobotMap.AddWallByLine(float64(response.Degree), float64(response.Distance))

	if response.Last {
		RobotMap.Print(nil)
		lastAction = "Scan"
		RobotMap.ContinueToNextArea()
		currentID = -1
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
}
