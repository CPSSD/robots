package maps

import (
	"RobotDriverProtocol"
	"fmt"
)

var lastAction string
var currentID int

// Initialises the RDP stuff.
func RDPInit() {
	fmt.Println("* Registering Response Handler")
	RobotDriverProtocol.RegisterResponseHandler(RDPConnector)
	RobotDriverProtocol.Init()
}

// Demonstrates the robot handling data from the responses.
func demo(){
	RDPConnector(RobotDriverProtocol.MoveResponse{RobotDriverProtocol.Response{0, 0, true}, 0, 40})
	RDPConnector(RobotDriverProtocol.RotateResponse{RobotDriverProtocol.Response{0, 0, true}, 90})
	RDPConnector(RobotDriverProtocol.MoveResponse{RobotDriverProtocol.Response{0, 0, true}, 0, 40})
	var distance uint16 = 50
	for i := 0; i < 360; i += 1 {
		RDPConnector(RobotDriverProtocol.ScanResponse{RobotDriverProtocol.Response{0, 0, true}, uint16(i), distance, false})
	}
	RDPConnector(RobotDriverProtocol.ScanResponse{RobotDriverProtocol.Response{0, 0, true}, 360, distance, true})
}

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
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type,"}")

	// Updates robots location to the response location.
	// Check reason, do something based on reason.
	RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
	lastAction = "Stop"
	currentID = -1

}