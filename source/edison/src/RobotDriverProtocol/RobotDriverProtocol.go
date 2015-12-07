package RobotDriverProtocol

import (
	"SPI"
	"time"
)

const (
	moveCode   = byte(0)
	stopCode   = byte(1)
	rotateCode = byte(2)
	scanCode   = byte(3)
)

// Response is a generic type which is used in the different Response types
type Response struct {
	ID     uint16
	Type   byte
	Reason string
}

// MoveResponse is a response type for Move calls
type MoveResponse struct {
	Response
	Angle     uint16
	Magnitude uint32
}

// StopResponse is a response type for Stop calls
type StopResponse struct {
	Response
	Angle     uint16
	Magnitude uint32
}

// RotateResponse is a response type for Rotate calls
type RotateResponse struct {
	Response
	Angle uint16
}

// ScanResponse is a response type for Scan calls
type ScanResponse struct {
	Response
	Degree   uint16
	Distance uint32
	Last     bool
}

var id uint16

// responseHandler is a function registered by the main program that will be called when a response is returned
var responseHandler func(interface{})

// ProcessResponse will process a response
func processResponse(responseBuffer []uint8) {
	// TODO: process responseBuffer, create the relevant response stuct and call responseHandler
	// Will need to talk with @ConorF to decide on a format for the SPI responses
}

func listenForResponses() {
	for {
		time.Sleep(100)
		response := SPI.ReceiveData(1) // Recieve one byte for the length of the response
		if response[0] != 0 {
			processResponse(append(response, SPI.ReceiveData(response[0])))
		}
	}
}

// Init will Initialize the RobotDriverProtocol and spawn a Goroutine to listen for input over SPI
func Init() {
	go listenForResponses()
}

// RegisterResponseHandler is used to register a function which will be called when a response is received over SPI
func RegisterResponseHandler(newResponseHandler func(interface{})) {
	responseHandler = newResponseHandler
}

// Move implements the RDP "Move" command
func Move(angle uint16, magnitude uint32) {
	id++
	length := byte(10) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle and 4 bytes for the magnitude
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = moveCode
	data[4] = uint8((angle >> 8) & 0xFF)
	data[5] = uint8(angle & 0xFF)
	data[6] = uint8((magnitude >> 24) & 0xFF)
	data[7] = uint8((magnitude >> 16) & 0xFF)
	data[8] = uint8((magnitude >> 8) & 0xFF)
	data[9] = uint8(magnitude & 0xFF)
	SPI.TransferAndReceiveData(data)
	processResponse(data)
}

// Stop implements the RDP "Stop" command
func Stop() {
	id++
	length := byte(4) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = stopCode
	SPI.TransferAndReceiveData(data)
	processResponse(data)
}

// Rotate implements the RDP "Rotate" command
func Rotate(angle uint16) {
	id++
	length := byte(6) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = moveCode
	data[4] = uint8((angle >> 8) & 0xFF)
	data[5] = uint8(angle & 0xFF)
	SPI.TransferAndReceiveData(data)
	processResponse(data)
}

// Scan implements the RDP "Scan" command
func Scan() {
	id++
	length := byte(4) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = scanCode
	SPI.TransferAndReceiveData(data)
	processResponse(data)
}
