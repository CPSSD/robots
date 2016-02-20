#include "Arduino.h"
#include "structs.h"
#include "comms.h"
#include "math.h"

char comms::parseCommand() {
    incomingByte = Serial.read();
    return(char)incomingByte;
}

struct MoveCommand comms::constructMoveCommand() {
	MoveCommand moveCom = {0, 0, 0};
	while(Serial.peek() != 44) { //Next char is not " , "
      //As Serial.read() returns single chars as ASCII values, an id of 127 would be read as 49 then 50 then 55.
      //To correct, we shift current id one place to right, read next number, subtract 48 to extract actual value from ASCII representation, then add to id
	  Serial.println(Serial.peek());
      moveCom.id = moveCom.id * 10 + (Serial.read() - 48);
	  Serial.println(Serial.peek());
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 44) {
      moveCom.angle = moveCom.angle * 10 + (Serial.read() - 48);
    }
    moveCom.angle = (moveCom.angle * PI) / 180.0;
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      moveCom.magnitude = moveCom.magnitude * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    return moveCom;
}

void comms::constructStopCommand() {
	
}

struct RotateCommand comms::constructRotateCommand() {
	RotateCommand rotCom;
	while(Serial.peek() != 44) {
      rotCom.id = rotCom.id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) {
      rotCom.angle = rotCom.angle * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
	return rotCom;
}

void comms::constructScanCommand() {
	
}

void comms::serialReply(struct MoveResponse moveResp) {
    Serial.print("RESULT:\t");
    Serial.print(moveResp.id);
    Serial.print(",\t");
    Serial.print(((moveResp.angle) * 180.0) / PI);
    Serial.print(",\t");
    Serial.print(moveResp.magnitude);
    Serial.print(",\t");
    Serial.println(moveResp.reason);
}

void comms::serialReply(struct RotateResponse rotResp) {
    Serial.print("RESULT:\t");
    Serial.print(rotResp.id);
    Serial.print(",\t");
    Serial.print(rotResp.angle);
    Serial.print(",\t");
    Serial.println(rotResp.reason);
}

void comms::serialReply(struct ScanResponse scanResp) {
    Serial.print("RESULT:\t");
    Serial.print(scanResp.id);
    Serial.print(",\t");
    Serial.print(scanResp.angle);
    Serial.print(",\t");
    Serial.print(scanResp.distance);
    Serial.print(",\t");
    Serial.println(scanResp.last);
}