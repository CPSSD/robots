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
		case WaitingToBegin: // If we receive 255 move to SendingLength state 
							 // and put length of command to send in SPDR
			break;
		case SendingLength:  // byteReceived is the length of the command we are about to receive
			break;
		case SendingCommand: // if the number of bytes we have sent/receive >= sendingCommandLength and 
							 // receivingCommandLength then we are done, process the command
			break;
		case CheckingIfFinished: // If we last sent 1 or byteReceived is 1 then we change state to sendingCommand. 
							     // Else go to waiting
			break;
	}
}
