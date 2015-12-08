#include "SPI_Wrapper.h"

#include <SPI.h>
#include "Arduino.h"

#define InitialTransferByte 100

using namespace std;

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

void SPI_Wrapper::registerResponseHandler(SPI_Command_Handler newCommandHandler)
{
	commandHandler = newCommandHandler;
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
				SPDR = getLengthOfNextCommand();
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
