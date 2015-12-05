package maps

import (
	"RobotDriverProtocol"
	"fmt"
)

func RDPInit(){
	fmt.Println("* Registering Response Handler");
	RobotDriverProtocol.RegisterResponseHandler(RDPConnector)
	RDPConnector(RobotDriverProtocol.MoveResponse{RobotDriverProtocol.Response{1, 0, "NA"}, 35, 100})
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

}

func scanResponse(response RobotDriverProtocol.ScanResponse){
	fmt.Print("[Scan Response] Degree:", response.Degree, "// Distance:", response.Distance)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Add a wall at the specific location.
	// When last response, find next location to move to in map.go

}

func rotateResponse(response RobotDriverProtocol.RotateResponse){
	fmt.Print("[Rotate Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Changes the rotation of the robot

}

func stopResponse(response RobotDriverProtocol.StopResponse){
	fmt.Print("[Stop Response] Angle:", response.Angle)
	fmt.Println(" [Response] { ID:", response.ID, " // Type:", response.Type, "// Reason:", response.Reason,"}")

	// Updates robots location to the response location.
	// Check reason, do something based on reason.

}