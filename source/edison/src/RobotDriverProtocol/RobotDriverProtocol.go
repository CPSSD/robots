package RobotDriverProtocol

import (
	"SPI"
	"fmt"
	"time"
)

const (
	moveCode         = byte(1)
	stopCode         = byte(2)
	rotateCode       = byte(3)
	scanCode         = byte(4)
	initiateTransfer = byte(147)
)

// Response is a generic type which is used in the different Response types
type Response struct {
	ID      uint16
	Type    byte
	Success bool
}

// MoveResponse is a response type for Move calls
type MoveResponse struct {
	Response
	Angle     uint16
	Magnitude uint16
}

// StopResponse is a response type for Stop calls
type StopResponse struct {
	Response
	Angle     uint16
	Magnitude uint16
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
	Distance uint16
	Last     bool
}

var id uint16
var buffersToSend [][]uint8

// responseHandler is a function registered by the main program that will be called when a response is returned
var responseHandler func(interface{})

func processMoveResponse(responseBuffer []uint8) {
	if len(responseBuffer) < 7 {
		return
	}
	var moveResponse MoveResponse
	moveResponse.Type = 1
	moveResponse.ID = (uint16(responseBuffer[0]) << 8) + uint16(responseBuffer[1])
	moveResponse.Magnitude = (uint16(responseBuffer[2]) << 8) + uint16(responseBuffer[3])
	moveResponse.Angle = (uint16(responseBuffer[4]) << 8) + uint16(responseBuffer[5])
	moveResponse.Success = (responseBuffer[6] > 0)
	responseHandler(moveResponse)
}

func processStopResponse(responseBuffer []uint8) {
	if len(responseBuffer) < 7 {
		return
	}
	var stopResponse StopResponse
	stopResponse.Type = 2
	stopResponse.ID = (uint16(responseBuffer[0]) << 8) + uint16(responseBuffer[1])
	stopResponse.Magnitude = (uint16(responseBuffer[2]) << 8) + uint16(responseBuffer[3])
	stopResponse.Angle = (uint16(responseBuffer[4]) << 8) + uint16(responseBuffer[5])
	stopResponse.Success = (responseBuffer[6] > 0)
	responseHandler(stopResponse)
}

func processRotateResponse(responseBuffer []uint8) {
	if len(responseBuffer) < 5 {
		return
	}
	var rotateResponse RotateResponse
	rotateResponse.Type = 3
	rotateResponse.ID = (uint16(responseBuffer[0]) << 8) + uint16(responseBuffer[1])
	rotateResponse.Angle = (uint16(responseBuffer[2]) << 8) + uint16(responseBuffer[3])
	rotateResponse.Success = (responseBuffer[4] > 0)
	responseHandler(rotateResponse)
}

func processScanResponse(responseBuffer []uint8) {
	if len(responseBuffer) < 8 {
		return
	}
	var scanResponse ScanResponse
	scanResponse.Type = 4
	scanResponse.ID = (uint16(responseBuffer[0]) << 8) + uint16(responseBuffer[1])
	scanResponse.Last = (responseBuffer[2] > 0)
	scanResponse.Distance = (uint16(responseBuffer[3]) << 8) + uint16(responseBuffer[4])
	scanResponse.Degree = (uint16(responseBuffer[5]) << 8) + uint16(responseBuffer[6])
	scanResponse.Success = (responseBuffer[7] > 0)
	fmt.Println("Passing scanResponse to responseHandler")
	responseHandler(scanResponse)
}

// processResponse will process a response
func processResponse(responseBuffer []uint8) {
	fmt.Println(responseBuffer)
	switch responseBuffer[0] {
	case 1: //move response
		processMoveResponse(responseBuffer[1:])
	case 2: // stop response
		processStopResponse(responseBuffer[1:])
	case 3: // rotate response
		processRotateResponse(responseBuffer[1:])
	case 4: // scan response
		processScanResponse(responseBuffer[1:])
	}
}

// Sends one command, returns a boolean whether either device has more commands to send
func sendNextCommand() bool {
	length := uint8(0)
	if len(buffersToSend) > 0 {
		length = uint8(len(buffersToSend[0]))
	}

	data := make([]uint8, 1)
	data[0] = length
	time.Sleep(time.Millisecond)
	SPI.TransferAndReceiveData(data)
	if data[0] > length {
		length = data[0]
	}
	if length > 0 {
		fmt.Println("Length > 0")
		fmt.Println(length)

		dataBuffer := make([]uint8, length)
		if len(buffersToSend) > 0 {
			copy(dataBuffer, buffersToSend[0])
			buffersToSend = buffersToSend[1:]
		}

		time.Sleep(time.Millisecond)

		for i := 0; i < len(dataBuffer); i++ {
			time.Sleep(time.Millisecond)
			data[0] = dataBuffer[i]
			SPI.TransferAndReceiveData(data)
			dataBuffer[i] = data[0]
		}

		processResponse(dataBuffer)

		// repeat if we have more data to send
		data[0] = 0

		notFinished := false
		if len(buffersToSend) > 0 {
			data[0] = 1
			notFinished = true
		}

		time.Sleep(time.Millisecond)
		SPI.TransferAndReceiveData(data)

		return (notFinished == true || data[0] == 1)
	}
	return false
}

func spiLoop() {
	for {
		time.Sleep(20 * time.Millisecond)
		data := make([]uint8, 1)
		data[0] = initiateTransfer
		SPI.TransferAndReceiveData(data)
		if data[0] == 100 {
			for {
				moreCommandsToSend := sendNextCommand()
				if !moreCommandsToSend {
					break
				}
			}
		}
	}
}

// Init will Initialize the RobotDriverProtocol and spawn a Goroutine to listen for input over SPI
func Init() {
	go spiLoop()
}

// RegisterResponseHandler is used to register a function which will be called when a response is received over SPI
func RegisterResponseHandler(newResponseHandler func(interface{})) {
	responseHandler = newResponseHandler
}

// Move implements the RDP "Move" command
func Move(angle uint16, magnitude uint32) uint16 {
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
	buffersToSend = append(buffersToSend, data)
	return id
}

// Stop implements the RDP "Stop" command
func Stop() uint16 {
	id++
	length := byte(4) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = stopCode
	buffersToSend = append(buffersToSend, data)
	return id
}

// Rotate implements the RDP "Rotate" command
func Rotate(angle uint16) uint16 {
	id++
	length := byte(6) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = moveCode
	data[4] = uint8((angle >> 8) & 0xFF)
	data[5] = uint8(angle & 0xFF)
	buffersToSend = append(buffersToSend, data)
	return id
}

// Scan implements the RDP "Scan" command
func Scan() uint16 {
	id++
	length := byte(4) // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	data := make([]uint8, length)
	data[0] = length
	data[1] = uint8((id >> 8) & 0xFF)
	data[2] = uint8(id & 0xFF)
	data[3] = scanCode
	buffersToSend = append(buffersToSend, data)
	return id
}
