#include <SPI.h>

#include "SPI_Wrapper.h"
#include "Arduino.h"

#define moveCode 1
#define stopCode 2
#define rotateCode 3
#define scanCode 4
#define compassCode 5

#define InitialTransferByte 100

SPI_Move_Command_Handler SPI_Wrapper::moveCommandHandler = NULL;
SPI_Stop_Command_Handler SPI_Wrapper::stopCommandHandler = NULL;
SPI_Rotate_Command_Handler SPI_Wrapper::rotateCommandHandler = NULL;
SPI_Scan_Command_Handler SPI_Wrapper::scanCommandHandler = NULL;
SPI_Compass_Command_Handler SPI_Wrapper::compassCommandHandler = NULL;

QueueList<uint8_t> SPI_Wrapper::dataOutQueue = QueueList<uint8_t>();
uint8_t SPI_Wrapper::sendingCommandLength = 0;
		
State SPI_Wrapper::currentState = WaitingToBegin;
		
uint8_t SPI_Wrapper::commandBuffer[MAX_COMMAND_LENGTH] = {};
uint8_t SPI_Wrapper::receivingCommandLength = 0;
uint8_t SPI_Wrapper::commandBytesReceived = 0;  

void SPI_Wrapper::init() 
{
    SPISettings(100000, MSBFIRST, SPI_MODE0);
       
    SPCR |= _BV(SPE);
	
    pinMode(MISO, OUTPUT);
    SPI.attachInterrupt();

	SPDR = InitialTransferByte;
    currentState = WaitingToBegin;
	dataOutQueue.setPrinter(Serial);
}

void SPI_Wrapper::sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) {
	uint8_t length = 8; // 1 byte for the command number, 2 bytes for the ID, 2 for magnitude, 2 for angle, 1 for the status
	dataOutQueue.push(length);
	dataOutQueue.push(moveCode);
	dataOutQueue.push((uint8_t)(uniqueID >> 8)); 
	dataOutQueue.push((uint8_t)uniqueID);
	dataOutQueue.push((uint8_t)(magnitude >> 8));
	dataOutQueue.push((uint8_t)(magnitude));
	dataOutQueue.push((uint8_t)(angle >> 8));
	dataOutQueue.push((uint8_t)(angle));
	dataOutQueue.push((uint8_t)(status));
}	

void SPI_Wrapper::sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) 
{
	uint8_t length = 8; // 1 byte for the command number, 2 bytes for the ID, 2 for magnitude, 2 for angle, 1 for the status
	dataOutQueue.push(length);
	dataOutQueue.push(stopCode);
	dataOutQueue.push((uint8_t)(uniqueID >> 8)); 
	dataOutQueue.push((uint8_t)uniqueID);
	dataOutQueue.push((uint8_t)(magnitude >> 8));
	dataOutQueue.push((uint8_t)(magnitude));
	dataOutQueue.push((uint8_t)(angle >> 8));
	dataOutQueue.push((uint8_t)(angle));
	dataOutQueue.push((uint8_t)(status));
}

void SPI_Wrapper::sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status) 
{
	uint8_t length = 6; // 1 byte for the command number, 2 bytes for the ID, 2 for angle, 1 for the status
	dataOutQueue.push(length);
	dataOutQueue.push(rotateCode);
	dataOutQueue.push((uint8_t)(uniqueID >> 8)); 
	dataOutQueue.push((uint8_t)uniqueID);
	dataOutQueue.push((uint8_t)(angle >> 8));
	dataOutQueue.push((uint8_t)(angle));
	dataOutQueue.push((uint8_t)(status));
}

void SPI_Wrapper::sendScanResponse(uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status)
{	
	if (dataOutQueue.count() > 300) {
		return;
	}
	uint8_t length = 9; // 1 byte for the command number, 2 bytes for the ID, 1 byte for Last, 2 for angle, 2 for magnitude, 1 for the status
	dataOutQueue.push(length);
	dataOutQueue.push(scanCode);
	dataOutQueue.push((uint8_t)(uniqueID >> 8)); 
	dataOutQueue.push((uint8_t)uniqueID);
	dataOutQueue.push((uint8_t)(last));
	dataOutQueue.push((uint8_t)(angle >> 8));
	dataOutQueue.push((uint8_t)(angle));
	dataOutQueue.push((uint8_t)(magnitude >> 8));
	dataOutQueue.push((uint8_t)(magnitude));
	dataOutQueue.push((uint8_t)(status));
}

void SPI_Wrapper::sendCompassResponse(uint16_t uniqueID, uint16_t angle, bool status)
{
	uint8_t length = 6; // 1 byte for the command number, 2 bytes for the ID, 2 for angle, 1 for the status
	dataOutQueue.push(length);
	dataOutQueue.push(compassCode);
	dataOutQueue.push((uint8_t)(uniqueID >> 8)); 
	dataOutQueue.push((uint8_t)uniqueID);
	dataOutQueue.push((uint8_t)(angle >> 8));
	dataOutQueue.push((uint8_t)(angle));
	dataOutQueue.push((uint8_t)(status));
}

void SPI_Wrapper::processReceivedCommand(int length)
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
		case compassCode:
		{
			compassCommand commandStruct;
			commandStruct.commandNumber = compassCode;
			commandStruct.uniqueID = ((uint16_t)(commandBuffer[1]) << 8) + (uint16_t)(commandBuffer[2]);
			if (compassCommandHandler) {
				(*compassCommandHandler)(commandStruct);
			}
			break;
		}
	}
}

void SPI_Wrapper::registerMoveCommandHandler(SPI_Move_Command_Handler newCommandHandler)
{
	moveCommandHandler = newCommandHandler;
}

void SPI_Wrapper::registerStopCommandHandler(SPI_Stop_Command_Handler newCommandHandler)
{
	stopCommandHandler = newCommandHandler;
}

void SPI_Wrapper::registerRotateCommandHandler(SPI_Rotate_Command_Handler newCommandHandler)
{
	rotateCommandHandler = newCommandHandler;
}

void SPI_Wrapper::registerScanCommandHandler(SPI_Scan_Command_Handler newCommandHandler)
{
	scanCommandHandler = newCommandHandler;
}

void SPI_Wrapper::registerCompassCommandHandler(SPI_Compass_Command_Handler newCommandHandler) 
{
	compassCommandHandler = newCommandHandler;
}

uint8_t SPI_Wrapper::getNextCommandByte()
{
	if (dataOutQueue.isEmpty() || (commandBytesReceived > sendingCommandLength)) {
		return 0;
	}
	uint8_t commandByte = dataOutQueue.pop();
	return commandByte;
}

void SPI_Wrapper::spiIntteruptFunction()
{
    uint8_t byteReceived = SPDR;

	switch (currentState) {
		case WaitingToBegin:
		{
			// If we receive 147 move to SendingLength state 
			// and put length of command to send in SPDR
			if (byteReceived == 147) {
				commandBytesReceived = 0;
				sendingCommandLength = 0;
				
				sendingCommandLength = getNextCommandByte();
				SPDR = (byte)sendingCommandLength;
				currentState = SendingLength;
			} else {
				SPDR = InitialTransferByte;
			}
			break;
		}
		case SendingLength:  
		{
			// byteReceived is the length of the command we are about to receive
			receivingCommandLength = byteReceived;
			if ((receivingCommandLength == 0) && (sendingCommandLength == 0)) {
				// Nothing to send
				SPDR = InitialTransferByte;
				currentState = WaitingToBegin;
			} else {
				commandBytesReceived = 0;
				SPDR = (byte)getNextCommandByte();
				currentState = SendingCommand;
			}
			break;
		}
		case SendingCommand: 
		{
			// if the number of bytes we have sent/received >= sendingCommandLength and 
			// receivingCommandLength then we are done, process the command
			commandBuffer[commandBytesReceived] = byteReceived; 
			commandBytesReceived++;
			if ((commandBytesReceived >= sendingCommandLength) && (commandBytesReceived >= receivingCommandLength)) {
				processReceivedCommand(receivingCommandLength);
				SPDR = (dataOutQueue.isEmpty() ? 0 : 1);
				currentState = CheckingIfFinished;
			} else {
				SPDR = (byte)getNextCommandByte();
			}
			break;
		}
		case CheckingIfFinished: 
		{
			// If we last sent 1 or byteReceived is 1 then we change state to sendingCommand. 
			// Otherwise go to waiting
			if (!dataOutQueue.isEmpty() || (byteReceived == 1)) {
				sendingCommandLength = getNextCommandByte();
				SPDR = (byte)sendingCommandLength;
				currentState = SendingLength;
			} else {
				SPDR = InitialTransferByte;
				currentState = WaitingToBegin;
			}
			break;
		}
	}
}

// SPI interrupt
ISR(SPI_STC_vect) 
{
	SPI_Wrapper::spiIntteruptFunction();
}
