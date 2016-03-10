#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include "Arduino.h"
#include "Wire.h"
#include "stdint.h"
#include "Shared_Structs.h"

#define MAX_COMMAND_LENGTH 32
#define MAX_SLAVES 4

typedef void (*I2C_Move_Command_Handler)(moveCommand);
typedef void (*I2C_Stop_Command_Handler)(stopCommand);
typedef void (*I2C_Rotate_Command_Handler)(rotateCommand);
typedef void (*I2C_Scan_Command_Handler)(scanCommand);
typedef void (*I2C_Detect_Command_Handler)(detectCommand);

typedef void (*I2C_Move_Response_Handler)(moveResponse);
typedef void (*I2C_Stop_Response_Handler)(stopResponse);
typedef void (*I2C_Rotate_Response_Handler)(rotateResponse);
typedef void (*I2C_Scan_Response_Handler)(scanResponse);
typedef void (*I2C_Detect_Response_Handler)(detectResponse);

class I2C_Wrapper {
    public:
		static void init(I2C_Mode mode, uint8_t deviceId);
		
		// Sending commands
		static uint16_t sendMoveCommand(uint8_t slaveId, uint16_t angle, uint32_t magnitude);
		static uint16_t sendStopCommand(uint8_t slaveId);
		static uint16_t sendRotateCommand(uint8_t slaveId, uint16_t angle);
		static uint16_t sendScanCommand(uint8_t slaveId);
		static uint16_t sendDetectCommand(uint8_t slaveId, uint16_t rangeBegin, uint16_t rangeEnd, uint32_t maxDistance);
		
		// Handling receiving of commands
		static void registerMoveCommandHandler(I2C_Move_Command_Handler newCommandHandler);
		static void registerStopCommandHandler(I2C_Stop_Command_Handler newCommandHandler);
		static void registerRotateCommandHandler(I2C_Rotate_Command_Handler newCommandHandler);
		static void registerScanCommandHandler(I2C_Scan_Command_Handler newCommandHandler);
		static void registerDetectCommandHandler(I2C_Detect_Command_Handler newCommandHandler);
		
		// Response functions
		static void sendMoveResponse(uint8_t masterId, uint16_t uniqueID, uint32_t magnitude, uint16_t angle, bool status);		
		static void sendStopResponse(uint8_t masterId, uint16_t uniqueID, uint32_t magnitude, uint16_t angle, bool status);
		static void sendRotateResponse(uint8_t masterId, uint16_t uniqueID, uint16_t angle, bool status);
		static void sendScanResponse(uint8_t masterId, uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status);
		static void sendDetectResponse(uint8_t masterId, uint16_t uniqueID, uint16_t angle, uint32_t distane, bool status);
		
		// Handling receiving of responses
		static void registerMoveResponseHandler(I2C_Move_Response_Handler newResponseHandler);
		static void registerStopResponseHandler(I2C_Stop_Response_Handler newResponseHandler);
		static void registerRotateResponseHandler(I2C_Rotate_Response_Handler newResponseHandler);
		static void registerScanResponseHandler(I2C_Scan_Response_Handler newResponseHandler);
		static void registerDetectResponseHandler(I2C_Detect_Response_Handler newResponseHandler);
		
		static void i2cOnReceive(int numBytes); // called when data is received over I2C
		
	private:		
		static void processReceivedCommand(int length); // Processes the last recieved command, creating the right struct and calls commandHandler
		static void processReceivedResponse(int length); // Processes the last received response, creating the right response struct and calls the responsHandler
		
		static I2C_Move_Command_Handler moveCommandHandler;
		static I2C_Stop_Command_Handler stopCommandHandler;
		static I2C_Rotate_Command_Handler rotateCommandHandler;
		static I2C_Scan_Command_Handler scanCommandHandler;
		static I2C_Detect_Command_Handler detectCommandHandler;
		
		static I2C_Move_Response_Handler moveResponseHandler;
		static I2C_Stop_Response_Handler stopResponseHandler;
		static I2C_Rotate_Response_Handler rotateResponseHandler;
		static I2C_Scan_Response_Handler scanResponseHandler;
		static I2C_Detect_Response_Handler detectResponseHandler;
	
		static uint8_t dataOutBuffer[MAX_COMMAND_LENGTH];
		
		static I2C_Mode currentMode;
		static uint16_t currentID;
		static uint8_t deviceNumber;
        static State currentState;
		
		static uint8_t commandBuffer[MAX_COMMAND_LENGTH];
		static uint8_t commandOutBuffer[MAX_COMMAND_LENGTH];
};

#endif
