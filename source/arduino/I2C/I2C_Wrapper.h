#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include "Arduino.h"
#inclue "Wire.h"
#include "stdint.h"

#define MAX_BUFFER_SIZE 256
#define MAX_COMMAND_LENGTH 32

typedef struct {
	uint8_t commandNumber;
	uint16_t uniqueID;
} command;

typedef struct moveCommand : command {
	uint16_t angle;
	uint32_t magnitude;
} moveCommand;

typedef struct stopCommand : command {
	// No extra information for stop command
} stopCommand;

typedef struct rotateCommand : command {
	uint16_t angle;
} rotateCommand;

typedef struct {
	uint8_t commandNumber;
	uint16_t uniqueID;
	bool success;
} response;

typedef struct moveResponse : response {
	uint16_t angle;
	uint32_t magnitude;
} moveResponse;

typedef struct stopResponse : response {
	uint16_t angle;
	uint32_t magnitude;
} stopResponse;

typedef struct rotateResponse : response {
	uint16_t angle;
} rotateResponse;


typedef enum {
	Master = 0,
	Slave = 1
} I2C_Mode;

typedef enum {
	WaitingToBegin = 0,
	SendingLength = 1,
	SendingCommand = 2,
	CheckingIfFinished = 3
} I2C_state;


typedef void (*I2C_Move_Command_Handler)(moveCommand);
typedef void (*I2C_Stop_Command_Handler)(stopCommand);
typedef void (*I2C_Rotate_Command_Handler)(rotateCommand);

typedef void (*I2C_Move_Response_Handler)(moveResponse);
typedef void (*I2C_Stop_Response_Handler)(stopResponse);
typedef void (*I2C_Rotate_Response_Handler)(rotateResponse);

class I2C_Wrapper {
    public:
		static void init(I2C_Mode mode, uint8_t deviceId);
		
		// Sending commands
		static uint16_t sendMoveCommand(uint16_t angle, uint32_t magnitude);
		static uint16_t sendStopCommand();
		static uint16_t sendRotateCommand(uint16_t angle);
		
		// Handling receiving of commands
		static void registerMoveCommandHandler(I2C_Move_Command_Handler newCommandHandler);
		static void registerStopCommandHandler(I2C_Stop_Command_Handler newCommandHandler);
		static void registerRotateCommandHandler(I2C_Rotate_Command_Handler newCommandHandler);
		
		// Response functions
		static void sendMoveResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);		
		static void sendStopResponse(uint16_t uniqueID, uint16_t magnitude, uint16_t angle, bool status);
		static void sendRotateResponse(uint16_t uniqueID, uint16_t angle, bool status);
		
		// Handling receiving of responses
		static void registerMoveResponseHandler(I2C_Move_Response_Handler newResponseHandler);
		static void registerStopResponseHandler(I2C_Stop_Response_Handler newResponseHandler);
		static void registerRotateResponseHandler(I2C_Rotate_Response_Handler newResponseHandler);
		
		static void i2cOnReceive(int numBytes); // called when data is received over I2C
		static void i2cOnRequest(); // called when data is requested by the master
		static void stepI2C(); // should be called every loop() in Arduino so that saved commands can be sent and the slave can be polled for responses
		
	private:		
		static void processReceivedCommand(int length); // Processes the last recieved command, creating the right struct and calls responseHandler
		static uint8_t getNextCommandByte();
		
		static I2C_Move_Command_Handler moveCommandHandler;
		static I2C_Stop_Command_Handler stopCommandHandler;
		static I2C_Rotate_Command_Handler rotateCommandHandler;
		
		static I2C_Move_Response_Handler moveResponseHandler;
		static I2C_Stop_Response_Handler stopResponseHandler;
		static I2C_Rotate_Command_Handler rotateResponseHandler;
	
		// Each command should be stored in the buffer preceeded by a byte telling us the length of the command
		static uint8_t dataOutBuffer[MAX_BUFFER_SIZE]; // Circular buffer contatining each byte to send
		static int bufferOutFillBegin;
		static int bufferOutFillEnd;
		static uint8_t sendingCommandLength;
		
		static I2C_Mode currentMode;
		static uint16_t currentID;
		static uint8_t deviceNumber;
        static I2C_state currentState;
		
		static uint8_t commandBuffer[MAX_COMMAND_LENGTH];
		static uint8_t receivingCommandLength;
		static uint8_t commandBytesReceived;       		
};

#endif