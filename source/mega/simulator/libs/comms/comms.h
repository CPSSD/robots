#ifndef comms_h
#define comms_h

#include "Arduino.h"
#include "structs.h"

class comms {
  public:
	char incomingByte;
	
    char parseCommand();
	struct MoveCommand constructMoveCommand();
	void constructStopCommand();
	struct RotateCommand constructRotateCommand();
	void constructScanCommand();
	void serialReply(struct MoveResponse moveResp);
	void serialReply(struct RotateResponse rotResp);
	void serialReply(struct ScanResponse scanResp);
};

#endif