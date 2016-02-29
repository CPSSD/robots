#include "I2C_Wrapper.h"
#include "Arduino.h"
#include "Wire.h"

#define moveCode 1
#define stopCode 2
#define rotateCode 3
#define scanCode 4
#define detectCode 5

I2C_Move_Command_Handler I2C_Wrapper::moveCommandHandler = NULL;
I2C_Stop_Command_Handler I2C_Wrapper::stopCommandHandler = NULL;
I2C_Rotate_Command_Handler I2C_Wrapper::rotateCommandHandler = NULL;
I2C_Scan_Command_Handler I2C_Wrapper::scanCommandHandler = NULL;
I2C_Detect_Command_Handler I2C_Wrapper::detectCommandHandler = NULL;

I2C_Move_Response_Handler I2C_Wrapper::moveResponseHandler = NULL;
I2C_Stop_Response_Handler I2C_Wrapper::stopResponseHandler = NULL;
I2C_Rotate_Response_Handler I2C_Wrapper::rotateResponseHandler = NULL;
I2C_Scan_Response_Handler I2C_Wrapper::scanResponseHandler = NULL;
I2C_Detect_Response_Hanlder I2C_Wrapper::detectResponseHandler = NULL;

uint8_t I2C_Wrapper::deviceNumber = 0;
uint16_t I2C_Wrapper::currentID = 0;

uint8_t I2C_Wrapper::dataOutBuffer[MAX_BUFFER_SIZE] = {};
int I2C_Wrapper::bufferOutFillBegin = 0;
int I2C_Wrapper::bufferOutFillEnd = 0;
uint8_t I2C_Wrapper::sendingCommandLength = 0;
		
I2C_Mode I2C_Wrapper::currentMode = Master;
I2C_state I2C_Wrapper::currentState = WaitingToBegin;
		
uint8_t I2C_Wrapper::commandBuffer[MAX_COMMAND_LENGTH] = {};
uint8_t I2C_Wrapper::commandOutBuffer[MAX_COMMAND_LENGTH] = {};

uint8_t I2C_Wrapper::slaveDevices[MAX_SLAVES];
uint8_t I2C_Wrapper::numberOfSlaves = 0;

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

void I2C_Wrapper::addSlave(uint8_t deviceId)
{
	if (numberOfSlaves < MAX_SLAVES) {
		slaveDevices[numberOfSlaves] = deviceId;
		numberOfSlaves++;
	}
}

// Sending responses when operating as a Slave
void I2C_Wrapper::sendMoveResponse(uint16_t uniqueID, uint32_t magnitude, uint16_t angle, bool status) {
	uint8_t length = 10; // 1 byte for the command number, 2 bytes for the ID, 4 for magnitude, 2 for angle, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = moveCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 24);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 16);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 9) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 10) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}	

void I2C_Wrapper::sendStopResponse(uint16_t uniqueID, uint32_t magnitude, uint16_t angle, bool status) 
{
	uint8_t length = 10; // 1 byte for the command number, 2 bytes for the ID, 4 for magnitude, 2 for angle, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = stopCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 24);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 16);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 9) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 10) % MAX_BUFFER_SIZE] = (uint8_t)(status);
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

void I2C_Wrapper::sendScanResponse(uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status)
{
	uint8_t length = 9; // 1 byte for the command number, 2 bytes for the ID, 2 for angle, 2 for the magnitude, 1 for last bool, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = scanCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(last);
	dataOutBuffer[(bufferOutFillEnd + 9) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}

void I2C_Wrapper::sendDetectResponse(uint16_t uniqueID, uint16_t angle, uint32_t distane, bool status)
{
	uint8_t length = 10; // 1 byte for the command number, 2 bytes for the ID, 2 for angle, 4 for the distance, 1 for the status
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = detectCode;
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID >> 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(angle >> 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(distance >> 24);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(distance >> 16);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(distance >> 8);
	dataOutBuffer[(bufferOutFillEnd + 9) % MAX_BUFFER_SIZE] = (uint8_t)(distance);
	dataOutBuffer[(bufferOutFillEnd + 10) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}

// Sending commands when operating as Master
uint16_t I2C_Wrapper::sendMoveCommand(uint8_t slaveId, uint16_t angle, uint32_t magnitude)
{
	// Send move command when operating as master
	currentID++;
	uint8_t length = 10; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle and 4 bytes for the magnitude
	dataOutBuffer[0] = length;
	dataOutBuffer[1] = (uint8_t)((currentID >> 8));
	dataOutBuffer[2] = (uint8_t)currentID;
	dataOutBuffer[3] = moveCode;
	dataOutBuffer[4] = (uint8_t)(angle >> 8);
	dataOutBuffer[5] = (uint8_t)angle;
	dataOutBuffer[6] = (uint8_t)(magnitude >> 24);
	dataOutBuffer[7] = (uint8_t)(magnitude >> 16);
	dataOutBuffer[8] = (uint8_t)(magnitude >> 8);
	dataOutBuffer[9] = (uint8_t)magnitude;
	
	Wire.beginTransmission(slaveId);
	Wire.write(dataOutBuffer, length);
	Wire.endTransmission();
	return currentID;
}

uint16_t I2C_Wrapper::sendStopCommand(uint8_t slaveId)
{
	// Send stop command when operating as master
	currentID++;
	uint8_t length = 4; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	dataOutBuffer[0] = length;
	dataOutBuffer[1] = (uint8_t)((currentID >> 8));
	dataOutBuffer[2] = (uint8_t)currentID;
	dataOutBuffer[3] = stopCode;
	
	Wire.beginTransmission(slaveId);
	Wire.write(dataOutBuffer, length);
	Wire.endTransmission();
	return currentID;
}	
	
uint16_t I2C_Wrapper::sendRotateCommand(uint8_t slaveId, uint16_t angle)
{
	// Send rotate command when operating as master
	currentID++;
	uint8_t length = 6; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code, 2 bytes for the angle
	dataOutBuffer[0] = length;
	dataOutBuffer[1] = (uint8_t)((currentID >> 8));
	dataOutBuffer[2] = (uint8_t)currentID;
	dataOutBuffer[3] = rotateCode;
	dataOutBuffer[4] = (uint8_t)(angle >> 8);
	dataOutBuffer[5] = (uint8_t)angle;

	Wire.beginTransmission(slaveId);
	Wire.write(dataOutBuffer, length);
	Wire.endTransmission();
	return currentID;
}

uint16_t I2C_Wrapper::sendScanCommand(uint8_t slaveId)
{
	// Send scan command when operating as master
	currentID++;
	uint8_t length = 4; // 1 byte for the length of the request, 2 bytes for the id, 1 byte for the code
	dataOutBuffer[0] = length;
	dataOutBuffer[1] = (uint8_t)((currentID >> 8));
	dataOutBuffer[2] = (uint8_t)currentID;
	dataOutBuffer[3] = scanCode;
	
	Wire.beginTransmission(slaveId);
	Wire.write(dataOutBuffer, length);
	Wire.endTransmission();
	return currentID;
}

uint16_t I2C_Wrapper::sendDetectCommand(uint8_t slaveId, uint16_t rangeBegin, uint16_t rangeEnd, uint32_t maxDistance)
{
	currentID++;
	uint8_t length = 12; // 1 byte for length, 1 byte for command number, 2 bytes for ID, 2 bytes for rangeBegin, 2 bytes for rangeEnd, 4 bytes for distance
	dataOutBuffer[0] = length;
	dataOutBuffer[1] = (uint8_t)((currentID >> 8));
	dataOutBuffer[2] = (uint8_t)currentID;
	dataOutBuffer[3] = detectCode;
	dataOutBuffer[4] = (uint8_t)(rangeBegin >> 8);
	dataOutBuffer[5] = (uint8_t)rangeBegin;
	dataOutBuffer[6] = (uint8_t)(rangeEnd >> 8);
	dataOutBuffer[7] = (uint8_t)rangeEnd;
	dataOutBuffer[8] = (uint8_t)(maxDistance >> 24);
	dataOutBuffer[9] = (uint8_t)(maxDistance >> 16);
	dataOutBuffer[10] = (uint8_t)(maxDistance >> 8);
	dataOutBuffer[11] = (uint8_t)maxDistance;
	
	Wire.beginTransmission(slaveId);
	Wire.write(dataOutBuffer, length);
	Wire.endTransmission();
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
			break;
		}
		case stopCode:
		{
			stopCommand commandStruct;
			commandStruct.commandNumber = stopCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			
			if (stopCommandHandler) {
				(*stopCommandHandler)(commandStruct);
			}
			break;
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
			break;
		}
		case scanCode:
		{
			scanCommand commandStruct;
			commandStruct.commandNumber = scanCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			
			if (scanCommandHandler) {
				(*scanCommandHandler)(commandStruct);
			}
			break;
		}
		case detectCode:
		{
			detectCommand commandStruct;
			commandStruct.commandNumber = detectCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			commandStruct.rangeBegin = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			commandStruct.rangeEnd = ((uint16_t)(commandBuffer[6]) << 8) + (uint16_t)(commandBuffer[7]);
			commandStruct.maxDistance = (((uint32_t)(commandBuffer[8]) << 24) +
						                ((uint32_t)(commandBuffer[9]) << 16) +
						                ((uint32_t)(commandBuffer[10]) << 8) +
						                (uint32_t)(commandBuffer[11]));
			
			if (detectCommandHandler) {
				(*detectCommandHandler)(commandStruct);
			}
			break;
		}
	}
}

// Process a received response and create the right response struct
void I2C_Wrapper::processReceivedResponse(int length)
{
	// 0 is length
	switch (commandBuffer[1]) {
		case moveCode:
		{			
			moveResponse responseStruct;
			responseStruct.commandNumber = moveCode;
			responseStruct.uniqueID = ((uint16_t)(commandBuffer[2]) << 8) + (uint16_t)(commandBuffer[3]);
			responseStruct.magnitude = (((uint32_t)(commandBuffer[4]) << 24) +
									   ((uint32_t)(commandBuffer[5]) << 16) +
									   ((uint32_t)(commandBuffer[6]) << 8) +
									   (uint32_t)(commandBuffer[7]));
			responseStruct.angle = ((uint16_t)(commandBuffer[8]) << 8) + (uint16_t)(commandBuffer[9]);
			responseStruct.success = (commandBuffer[10] > 0);
									   
			if (moveResponseHandler) {
				(*moveResponseHandler)(responseStruct);
			}
			break;
		}
		case stopCode:
		{
			stopResponse responseStruct;
			responseStruct.commandNumber = stopCode;
			responseStruct.uniqueID = ((uint16_t)(commandBuffer[2]) << 8) + (uint16_t)(commandBuffer[3]);
			responseStruct.magnitude = (((uint32_t)(commandBuffer[4]) << 24) +
									   ((uint32_t)(commandBuffer[5]) << 16) +
									   ((uint32_t)(commandBuffer[6]) << 8) +
									   (uint32_t)(commandBuffer[7]));
			responseStruct.angle = ((uint16_t)(commandBuffer[8]) << 8) + (uint16_t)(commandBuffer[9]);
			responseStruct.success = (commandBuffer[10] > 0);
									   
			if (stopResponseHandler) {
				(*stopResponseHandler)(responseStruct);
			}
			break;
		}
		case rotateCode:
		{	
			rotateResponse responseStruct;
			responseStruct.commandNumber = rotateCode;
			responseStruct.uniqueID = ((uint16_t)(commandBuffer[2]) << 8) + (uint16_t)(commandBuffer[3]);
			responseStruct.angle = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			responseStruct.success = (commandBuffer[6] > 0);
			
			if (rotateResponseHandler) {
				(*rotateResponseHandler)(responseStruct);
			}
			break;
		}
		case scanCode:
		{
			scanResponse responseStruct;
			responseStruct.commandNumber = scanCode;
			responseStruct.uniqueID = ((uint16_t)(commandBuffer[2]) << 8) + (uint16_t)(commandBuffer[3]);
			responseStruct.angle = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			responseStruct.magnitude = ((uint16_t)(commandBuffer[6]) << 8) + (uint16_t)(commandBuffer[7]);
			responseStruct.last = commandBuffer[8] > 0;
			responseStruct.success = commandBuffer[9] > 0;
			
			if (scanResponseHandler) {
				(*scanResponseHandler)(responseStruct);
			}
			break;
		}
		case detectCode:
		{
			detectResponse responseStruct;
			responseStruct.commandNumber = detectCode;
			responseStruct.uniqueID = ((uint16_t)(commandBuffer[2]) << 8) + (uint16_t)(commandBuffer[3]);
			responseStruct.angle = ((uint16_t)(commandBuffer[4]) << 8) + (uint16_t)(commandBuffer[5]);
			responseStruct.distance = (((uint32_t)(commandBuffer[6]) << 24) +
									  ((uint32_t)(commandBuffer[7]) << 16) +
						              ((uint32_t)(commandBuffer[8]) << 8) +
						              (uint32_t)(commandBuffer[9]));
			responseStruct.success = commandBuffer[10] > 0;
			
			if (detectResponseHandler) {
				(*detectResponseHandler)(responseStruct);
			}
			break;
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

void I2C_Wrapper::registerScanCommandHandler(I2C_Scan_Command_Handler newCommandHandler)
{
	scanCommandHandler = newCommandHandler;
}

void I2C_Wrapper::registerDetectCommandHandler(I2C_Detect_Command_Handler newCommandHandler)
{
	detectCommandHandler = newCommandHandler;
}

void I2C_Wrapper::registerMoveResponseHandler(I2C_Move_Response_Handler newResponseHandler)
{
	moveResponseHandler = newResponseHandler;
}
	
void I2C_Wrapper::registerStopResponseHandler(I2C_Stop_Response_Handler newResponseHandler)
{
	stopResponseHandler = newResponseHandler;
}

void I2C_Wrapper::registerRotateResponseHandler(I2C_Rotate_Response_Handler newResponseHandler)
{
	rotateResponseHandler = newResponseHandler;
}

void I2C_Wrapper::registerDetectResponseHandler(I2C_Detect_Response_Handler newResponseHandler)
{
	detectResponseHandler = newResponseHandler;
}

uint8_t I2C_Wrapper::getNextCommandByte()
{
	if (bufferOutFillBegin == bufferOutFillEnd) {
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
	if (bufferOutFillBegin == bufferOutFillEnd) {
		Wire.write(0);
	} else {
		uint8_t length = getNextCommandByte(); // first byte in command gives us the length of the command
		commandOutBuffer[0] = length;
		
		for (uint8_t i = 1; i <= length; i++) {
			commandOutBuffer[i] = getNextCommandByte();
		} 
		
		Wire.write(commandOutBuffer, length + 1);
	}
}

void I2C_Wrapper::stepI2C() 
{
	// Need to call this every loop() so that we can poll the slave devices for responses
	if (currentMode != Master) {
		return;
	}
		
	for (int i = 0; i < numberOfSlaves; i++) {
		Wire.requestFrom(slaveDevices[i], (uint8_t)MAX_COMMAND_LENGTH);
		
		if (Wire.available()) {
			uint8_t length = Wire.read();
			
			if (length > 0) {
				commandBuffer[0] = length;
				for (int j = 1; j <= length; j++) {
					commandBuffer[j] = Wire.available() ? Wire.read() : 0;
				}
	
				processReceivedResponse(length);
			}
		}
	}
}

