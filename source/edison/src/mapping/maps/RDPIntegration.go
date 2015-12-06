package maps

import (
	"RobotDriverProtocol"
	"fmt"
)

var lastAction string
var currentID int

func RDPInit(){
	fmt.Println("* Registering Response Handler");
	RobotDriverProtocol.RegisterResponseHandler(RDPConnector)
//	RDPConnector(RobotDriverProtocol.MoveResponse{RobotDriverProtocol.Response{0, 0, "Boo"}, 0, 40})
//	RDPConnector(RobotDriverProtocol.RotateResponse{RobotDriverProtocol.Response{0, 0, "Boo"}, 90})
//	RDPConnector(RobotDriverProtocol.MoveResponse{RobotDriverProtocol.Response{0, 0, "Boo"}, 0, 40})
//	for i := 0; i < 360; i+=8 {
//		RDPConnector(RobotDriverProtocol.ScanResponse{RobotDriverProtocol.Response{0, 0, "Boo"}, uint16(i), 50, false})
//	}
//	RDPConnector(RobotDriverProtocol.ScanResponse{RobotDriverProtocol.Response{0, 0, "Boo"}, 360, 50, true})
}

func RDPConnector(data interface{}){
	fmt.Println(data)
	fmt.Println("Data Recieved..")

	// Checks to see what Data Type the recieved Interface{} is.
	if _, ok := data.(RobotDriverProtocol.MoveResponse); ok {
		moveResponse(data.(RobotDriverProtocol.MoveResponse));
	} else if _, ok := data.(RobotDriverProtocol.RotateResponse); ok {
		rotateResponse(data.(RobotDriverProtocol.RotateResponse));
	} else if _, ok := data.(RobotDriverProtocol.StopResponse); ok {
		stopResponse(data.(RobotDriverProtocol.StopResponse));
	} else if _, ok := data.(RobotDriverProtocol.ScanResponse); ok {
		scanResponse(data.(RobotDriverProtocol.ScanResponse));
	}
}

func moveResponse(response RobotDriverProtocol.MoveResponse){
	fmt.Print("[Move Response] Angle:", response.Angle, " // Magnitude:", response.Magnitude)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Updates robots location to the responses location.
	RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
	lastAction = "Move"
	currentID = -1
}

func scanResponse(response RobotDriverProtocol.ScanResponse){
	fmt.Print("[Scan Response] Degree:", response.Degree, "// Distance:", response.Distance)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Add a wall at the specific location.
	// When last response, find next location to move to in map.go
	RobotMap.AddWallByLine(float64(response.Degree), float64(response.Distance))

	if response.Last {
		lastAction = "Scan"
		RobotMap.ContinueToNextArea()
		currentID = -1
	}

}

func rotateResponse(response RobotDriverProtocol.RotateResponse){
	fmt.Print("[Rotate Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Changes the rotation of the robot
	RobotMap.GetRobot().Rotate(float64(response.Angle))
	lastAction = "Rotate"
	currentID = -1
}

func stopResponse(response RobotDriverProtocol.StopResponse){
	fmt.Print("[Stop Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Updates robots location to the response location.
	// Check reason, do something based on reason.
	RobotMap.MoveRobotAlongLine(float64(response.Angle), float64(response.Magnitude))
	lastAction = "Stop"
	currentID = -1

}