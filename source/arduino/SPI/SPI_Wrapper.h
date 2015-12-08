#ifndef SPI_WRAPPER_H
#define SPI_WRAPPER_H

#include "Arduino.h"
#include "stdint.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_COMMAND_LENGTH 32

typedef struct {
	uint8_t commandNumber;
	uint16_t uniqueID;
} command;

typedef struct moveCommand : command {
	uint16_t angle;
	uint16_t magnitude;
} moveCommand;

typedef struct stopCommand : command {
	// No extra information for stop command
} stopCommand;

typedef struct rotateCommand : command {
	uint16_t angle;
} rotateCommand;

typedef struct scanCommand : command {
	// No extra information for scan command
} scanCommand;

typedef enum {
	WaitingToBegin,
	SendingLength,
	SendingCommand,
	CheckingIfFinished
} SPI_state;


typedef void (*SPI_Command_Handler)(command*);


class SPI_Wrapper {
    public:
		static void init();
		static void registerCommandHandler(SPI_Command_Handler newCommandHandler);
		static void sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);		
		static void sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);
		static void sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status);
		static void sendScanResponse(uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status);
		
    private:
		static void processReceivedCommand(int length); // Processes the last recieved command, creating the right struct and calls responseHandler
		
		static SPI_Command_Handler commandHandler;
	
		// Each command should be stored in the buffer preceeded by a byte telling us the length of the command
		static uint8_t dataOutBuffer[MAX_BUFFER_SIZE]; // Circular buffer contatining each byte to send
		static int bufferOutFillBegin;
		static int bufferOutFillEnd;
		static int sendingCommandLength;
		
        static SPI_state currentState;
		
		static uint8_t commandBuffer[MAX_COMMAND_LENGTH];
		static int receivingCommandLength;
		static int commandBytesReceived;        
		
		static uint8_t getNextCommandByte();
};

#endif
