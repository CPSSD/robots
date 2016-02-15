#include "I2C_Wrapper.h"
#include "Arduino.h"
#include "Wire.h"

#define moveCode 1
#define stopCode 2
#define rotateCode 3

#define InitialTransferByte 255

I2C_Move_Command_Handler I2C_Wrapper::moveCommandHandler = NULL;
I2C_Stop_Command_Handler I2C_Wrapper::stopCommandHandler = NULL;
I2C_Rotate_Command_Handler I2C_Wrapper::rotateCommandHandler = NULL;

uint8_t I2C_Wrapper::deviceNumber = 0;
uint16_t I2C_Wrapper::currentID = 0;

uint8_t I2C_Wrapper::dataOutBuffer[MAX_BUFFER_SIZE] = {};
int I2C_Wrapper::bufferOutFillBegin = 0;
int I2C_Wrapper::bufferOutFillEnd = 0;
uint8_t I2C_Wrapper::sendingCommandLength = 0;
		
I2C_Mode I2C_Wrapper::currentMode = Master;
I2C_state I2C_Wrapper::currentState = WaitingToBegin;
		
uint8_t I2C_Wrapper::commandBuffer[MAX_COMMAND_LENGTH] = {};
uint8_t I2C_Wrapper::commandBytesReceived = 0;  

void I2C_Wrapper::init(I2C_Mode mode, uint8_t deviceId)
{
	currentMode = mode;
    if (mode == Master) {
		Wire.begin();
	} else if (mode == Slave) {
		Wire.begin(deviceId);
		Wire.onReceive(i2cOnReceive);
		Wire.onRequest(i2cOnRequest);
	}
}

void I2C_Wrapper::sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) {
	uint8_t length = 8; // 1 byte for the command number, 2 bytes for the ID, 2 for magnitude, 2 for angle, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = moveCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}	

void I2C_Wrapper::sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) 
{
	uint8_t length = 8; // 1 byte for the command number, 2 bytes for the ID, 2 for magnitude, 2 for angle, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = stopCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}

void I2C_Wrapper::sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status) 
{
	uint8_t length = 6; // 1 byte for the command number, 2 bytes for the ID, 2 for angle, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = rotateCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}

static uint16_t sendMoveCommand(uint16_t angle, uint32_t magnitude)
{
	// Send move command when operating as master
	currentID++;
	uint8_t length = 10; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle and 4 bytes for the magnitude
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = (uint8_t)((currentID >> 8);
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)id;
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = moveCode;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)angle;
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 24);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 16);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 9) % MAX_BUFFER_SIZE] = (uint8_t)magnitude;
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
	return currentID;
}

static uint16_t sendStopCommand()
{
	// Send stop command when operating as master
	currentID++;
	uint8_t length = 4; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = (uint8_t)((currentID >> 8);
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)id;
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = moveCode;
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
	return currentID;
}	
	
static uint16_t sendRotateCommand(uint16_t angle)
{
	// Send rotate command when operating as master
	currentID++;
	uint8_t length = 6; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = (uint8_t)((currentID >> 8);
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)id;
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = moveCode;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)angle;
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
	return currentID;
}

// Process a received command and create the right command struct
void I2C_Wrapper::processReceivedCommand(int length)
{
	// 0 is length
	// 1 is id >> 8
	// 2 is id && FF
	switch (commandBuffer[3]) {
		case moveCode:
		{
			moveCommand commandStruct;
			commandStruct.commandNumber = moveCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			commandStruct.angle = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			commandStruct.magnitude = (((uint32_t)(commandBuffer[6]) << 24) +
									   ((uint32_t)(commandBuffer[7]) << 16) +
									   ((uint32_t)(commandBuffer[8]) << 8) +
									   (uint32_t)(commandBuffer[9]));
			if (moveCommandHandler) {
				(*moveCommandHandler)(commandStruct);
			}
		}
		case stopCode:
		{
			stopCommand commandStruct;
			commandStruct.commandNumber = stopCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			if (stopCommandHandler) {
				(*stopCommandHandler)(commandStruct);
			}
		}
		case rotateCode:
		{
			rotateCommand commandStruct;
			commandStruct.commandNumber = rotateCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			commandStruct.angle = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			if (rotateCommandHandler) {
				(*rotateCommandHandler)(commandStruct);
			}
		}
	}
}

void I2C_Wrapper::registerMoveCommandHandler(I2C_Move_Command_Handler newCommandHandler)
{
	moveCommandHandler = newCommandHandler;
}

void I2C_Wrapper::registerStopCommandHandler(I2C_Stop_Command_Handler newCommandHandler)
{
	stopCommandHandler = newCommandHandler;
}

void I2C_Wrapper::registerRotateCommandHandler(I2C_Rotate_Command_Handler newCommandHandler)
{
	rotateCommandHandler = newCommandHandler;
}

void I2C_Wrapper::registerMoveResponseHandler(I2C_Move_Response_Handler newResponseHandler)
{
	moveResponseHandler = newResponseHandler;
}
	
static void registerStopResponseHandler(I2C_Stop_Response_Handler newResponseHandler)
{
	stopResponseHandler = newResponseHandler;
}

void registerRotateResponseHandler(I2C_Rotate_Response_Handler newResponseHandler)
{
	rotateResponseHandler = newResponseHandler;
}

uint8_t I2C_Wrapper::getNextCommandByte()
{
	if ((bufferOutFillBegin == bufferOutFillEnd) || (commandBytesReceived > sendingCommandLength)) {
		return 0;
	}
	uint8_t commandByte = dataOutBuffer[bufferOutFillBegin];
	bufferOutFillBegin = (bufferOutFillBegin + 1) % MAX_BUFFER_SIZE;
	return commandByte;
}

void I2C_Wrapper::i2cOnReceive(int numBytes) 
{
	// Called when a command is recieved over I2C
	for (int i = 0; i < numBytes; i++) {
		commandBuffer[i] = Wire.read();
	}
	processReceivedCommand(numBytes);
}
	
		
void I2C_Wrapper::i2cOnRequest() 
{
	// Called when data is requested by the master
	if ((bufferOutFillBegin == bufferOutFillEnd) || (commandBytesReceived > sendingCommandLength)) {
		Wire.send(0);
	} else {
		uint8_t length = dataOutBuffer[bufferOutFillBegin];
		Wire.send(dataOutBuffer + bufferOutFillBegin, length);  
		bufferOutFillBegin = (bufferOutFillBegin + length) % MAX_BUFFER_SIZE;
	}
}

void I2C_Wrapper::stepI2C() 
{
	// should be called every loop() in Arduino so that saved commands can be sent and the slave can be polled for responses
}

