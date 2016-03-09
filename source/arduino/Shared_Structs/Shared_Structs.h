#ifndef SHARED_STRUCTS_H
#define SHARED_STRUCTS_H

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

typedef struct scanCommand : command {
	// No extra information for scan command
} scanCommand;

typedef struct detectCommand : command {
	uint16_t rangeBegin;
	uint16_t rangeEnd;
	uint32_t maxDistance;
} detectCommand;

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

typedef struct scanResponse : response {
	uint16_t angle;
	uint16_t magnitude;
	bool last;
} scanResponse;

typedef struct detectResponse : response {
	uint16_t angle;
	uint32_t distance;
} detectResponse;

typedef enum {
	Master = 0,
	Slave = 1
} I2C_Mode;

typedef enum {
	WaitingToBegin = 0,
	SendingLength = 1,
	SendingCommand = 2,
	CheckingIfFinished = 3
} State;

#endif