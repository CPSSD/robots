#include <SPI.h>

#include "SPI_Wrapper.h"
#include "Arduino.h"


#define InitialTransferByte 100

void SPI_Wrapper::init() 
{
    SPISettings(100000, MSBFIRST, SPI_MODE0);
    SPI.begin();
       
    SPCR |= bit(SPE);
    pinMode(MISO, OUTPUT);
    SPI.attachInterrupt();

	SPDR = InitialTransferByte;
	commandBytesReceived = 0;
    currentState = WaitingToBegin;
}

void SPI_Wrapper::sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) {
	uint8_t length = 8; // 1 byte for the command number, 2 bytes for the ID, 2 for magnitude, 2 for angle, 1 for the status
	// It might be better to use a helper function to make this easier to read and edit
	dataOutBuffer[bufferOutFillEnd] = length;
	dataOutBuffer[(bufferOutFillEnd + 1) % MAX_BUFFER_SIZE] = 1; // command number
	dataOutBuffer[(bufferOutFillEnd + 2) % MAX_BUFFER_SIZE] = (uint8_t)(uniqueID << 8); 
	dataOutBuffer[(bufferOutFillEnd + 3) % MAX_BUFFER_SIZE] = (uint8_t)uniqueID;
	dataOutBuffer[(bufferOutFillEnd + 4) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude << 8);
	dataOutBuffer[(bufferOutFillEnd + 5) % MAX_BUFFER_SIZE] = (uint8_t)(magnitude);
	dataOutBuffer[(bufferOutFillEnd + 6) % MAX_BUFFER_SIZE] = (uint8_t)(angle << 8);
	dataOutBuffer[(bufferOutFillEnd + 7) % MAX_BUFFER_SIZE] = (uint8_t)(angle);
	dataOutBuffer[(bufferOutFillEnd + 8) % MAX_BUFFER_SIZE] = (uint8_t)(status);
	bufferOutFillEnd = (bufferOutFillEnd + length + 1) % MAX_BUFFER_SIZE;
}	

void SPI_Wrapper::sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status) 
{
	// TODO: Implement
}

void SPI_Wrapper::sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status) 
{
	// TODO: Implement
}

void SPI_Wrapper::sendScanResponse(uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status)
{
	// TODO: Implement
}

void SPI_Wrapper::processReceivedCommand(int length)
{
	// TODO: Create the correct struct, parse the response and call the command handler
}

void SPI_Wrapper::registerCommandHandler(SPI_Command_Handler newCommandHandler)
{
	commandHandler = newCommandHandler;
}

uint8_t SPI_Wrapper::getNextCommandByte()
{
	if ((bufferOutFillBegin == bufferOutFillEnd) || (commandBytesReceived > sendingCommandLength)) {
		return 0;
	}
	uint8_t commandByte = dataOutBuffer[bufferOutFillBegin];
	bufferOutFillBegin = (bufferOutFillBegin + 1) % MAX_BUFFER_SIZE;
	return commandByte;
}

ISR(SPI_STC_vect) 
{
    uint8_t byteReceived = SPDR;

	switch (currentState) {
		case WaitingToBegin:
		{
			// If we receive 255 move to SendingLength state 
			// and put length of command to send in SPDR
			if (byteReceived == 255) {
				SPDR = getNextCommandByte();
				sendingCommandLength = (int)SPDR;
				currentState = SendingLength;
			} else {
				SPDR = InitialTransferByte;
			}
			break;
		}
		case SendingLength:  
		{
			// byteReceived is the length of the command we are about to receive
			receivingCommandLength = (int)byteReceived;
			if ((receivingCommandLength == 0) && sendingCommandLength == 0) {
				// Nothing to send
				SPDR = InitialTransferByte;
				currentState = WaitingToBegin;
			} else {
				commandBytesReceived = 0;
				SPDR = getNextCommandByte();
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
				SPDR = (bufferOutFillBegin == bufferOutFillEnd) ? 0 : 1;
				currentState = CheckingIfFinished;
			} else {
				SPDR = getNextCommandByte();
			}
			break;
		}
		case CheckingIfFinished: 
		{
			// If we last sent 1 or byteReceived is 1 then we change state to sendingCommand. 
			// Otherwise go to waiting
			if ((bufferOutFillBegin == bufferOutFillEnd) || (byteReceived == 1)) {
				SPDR = getLengthOfNextCommand();
				sendingCommandLength = (int)SPDR;
				currentState = SendingLength;
			} else {
				SPDR = InitialTransferByte;
				currentState = WaitingToBegin;
			}
			break;
		}
	}
}
