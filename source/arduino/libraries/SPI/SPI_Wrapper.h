#ifndef SPI_WRAPPER_H
#define SPI_WRAPPER_H

#include "Arduino.h"
#include "stdint.h"
#include "Shared_Structs.h"

#define MAX_BUFFER_SIZE 256
#define MAX_COMMAND_LENGTH 32

typedef void (*SPI_Move_Command_Handler)(moveCommand);
typedef void (*SPI_Stop_Command_Handler)(stopCommand);
typedef void (*SPI_Rotate_Command_Handler)(rotateCommand);
typedef void (*SPI_Scan_Command_Handler)(scanCommand);
typedef void (*SPI_Compass_Command_Handler)(compassCommand);

class SPI_Wrapper {
    public:
		static void init();
		static void registerMoveCommandHandler(SPI_Move_Command_Handler newCommandHandler);
		static void registerStopCommandHandler(SPI_Stop_Command_Handler newCommandHandler);
		static void registerRotateCommandHandler(SPI_Rotate_Command_Handler newCommandHandler);
		static void registerScanCommandHandler(SPI_Scan_Command_Handler newCommandHandler);
		static void registerCompassCommandHandler(SPI_Compass_Command_Handler newCommandHandler);
		
		// Response functions
		static void sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);		
		static void sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);
		static void sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status);
		static void sendScanResponse(uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status);
		static void sendCompassResponse(uint16_t uniqueID, uint16_t angle, bool status);
		
		static void spiIntteruptFunction(); // called in the SPI interrupt
		
	private:	
		static void processReceivedCommand(int length); // Processes the last recieved command, creating the right struct and calls responseHandler
		static uint8_t getNextCommandByte();
		
		static SPI_Move_Command_Handler moveCommandHandler;
		static SPI_Stop_Command_Handler stopCommandHandler;
		static SPI_Rotate_Command_Handler rotateCommandHandler;
		static SPI_Scan_Command_Handler scanCommandHandler;
		static SPI_Compass_Command_Handler compassCommandHandler;
	
		// Each command should be stored in the buffer preceeded by a byte telling us the length of the command
		static uint8_t dataOutBuffer[MAX_BUFFER_SIZE]; // Circular buffer contatining each byte to send
		static int bufferOutFillBegin;
		static int bufferOutFillEnd;
		static uint8_t sendingCommandLength;
		
        static State currentState;
		
		static uint8_t commandBuffer[MAX_COMMAND_LENGTH];
		static uint8_t receivingCommandLength;
		static uint8_t commandBytesReceived;       		
};

#endif
