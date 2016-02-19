#include <comms.h>
#include <structs.h>
#include <calc.h>
#include "math.h"

calc calculations;
comms communications;

const float SPEED = 0.1; //in mm per millisecond
const int STARTING_X = 20;
const int STARTING_Y = 20;
 //Use to access interrupt pins - may be needed to implement asynchronous stop
const byte INTERR_1 = 2;
const byte INTERR_2 = 3;

const MapLine MAP_BOUNDS[4] = {0, 0, 41, 0, 41, 0, 41, 41, 41, 41, 0, 41, 0, 41, 0, 0};
unsigned long moveTimer, rotateTimer, distTravelled;
int incomingByte, id, magnitude, angleInDegrees;
float angle;
point currentPosition, destination, nearestWall;
String instruct;
bool brakes, amMoving, amRotating;
MapLine ray;
EquationOfLine PERIMETER[4];
EquationOfLine equOfRay;

MoveCommand moveCom = {0, 0, 0};
MoveResponse moveResp = {0, 0, 0, 0};

RotateCommand rotCom = {0, 0};
RotateResponse rotResp = {0, 0, 0};

ScanResponse scanResp = {0, 0, 0, false};

void setup() {
  Serial.begin(9600);
  instruct = "";
  incomingByte = 0;
  id = 0;
  angle = 0.0;
  angleInDegrees = 0;
  magnitude = 0;
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  brakes = false;
  amMoving = false;
  amRotating = false;
  for(int i = 0; i < 4; i++) {
    PERIMETER[i] = calculations.getEquationOfLine(MAP_BOUNDS[i]);
  }
}

void loop() {
  
  if(Serial.available() > 0) {
    instruct += communications.parseCommand();
    delay(10); //Required to allow read buffer to recover
  }
    
  if(instruct == "moveRobot(") {
    //moveCom = communications.constructMoveCommand();
    makeMoveCommand();
    moveRobot(moveCom);
    //Reset global variables
    id = 0;
    angle = 0;
    magnitude = 0;
  }
  if(instruct == "rotate(") {
    //rotCom = communications.constructRotateCommand();
    makeRotateCommand();
    rotate(rotCom);
    id = 0;
    angle = 0;
  }
  /*if(instruct == "scan(") {
    makeScanCommand();
    do{
      scanResp = scan(id, (angleInDegrees * PI) / 180);
      serialReply(scanResp);
      angleInDegrees = angleInDegrees + 8; //This will only allow for scans between 0 and 357 degrees
    }
    while(!scanResp.last);
    //Reset global id variable
    id = 0;
    angleInDegrees = 0;
    instruct = "";
  }*/
  if(instruct == "stopRobot();") {
    stopRobot(); //Currently non-functional
  }
  if(brakes) {
    wrapUp(1);
    brakes = false;
  }
  if(amMoving && millis() - moveTimer >= 100UL) {
    moveResp = giveHeartbeatMove(moveCom, (millis() - moveTimer));
    //serialReply(moveResp);
    if(moveResp.reason > 0) {
      communications.serialReply(moveResp);
      currentPosition = destination;
    }
  }
  if(amRotating && millis() - rotateTimer >= 1000UL) {
    rotResp = giveHeartbeatRotate(rotCom);
  }
}

void makeMoveCommand() {
    while(Serial.peek() != 44) { //Next char is not " , "
      //As Serial.read() returns single chars as ASCII values, an id of 127 would be read as 49 then 50 then 55.
      //To correct, we shift current id one place to right, read next number, subtract 48 to extract actual value from ASCII representation, then add to id
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 44) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    angle = (angle * PI) / 180.0;
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      magnitude = magnitude * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
    moveCom = {id, angle, magnitude};
}

void makeRotateCommand() {
    while(Serial.peek() != 44) {
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
}

void moveRobot(struct MoveCommand com) {
  ray = calculations.makeLineFromPolar(com.angle, com.magnitude, currentPosition);
  equOfRay = calculations.getEquationOfLine(ray);
  destination = calculations.getDestination(ray, equOfRay, PERIMETER);
  com.magnitude = (int)calculations.getDistBetweenTwoPoints(equOfRay.xy, destination);
  amMoving = true;
  moveTimer = millis();
}

struct MoveResponse giveHeartbeatMove(struct MoveCommand com, unsigned long time) {
  distTravelled = distTravelled + ((SPEED * time) * 10);
  if(distTravelled >= com.magnitude) {
    amMoving = false;
    moveTimer = millis();
    return (MoveResponse){com.id, com.angle, com.magnitude, 1};
  }
  moveTimer = millis();
  return (MoveResponse){id, com.angle, (int)distTravelled, 0};
}

void stopRobot() {
  brakes = true;
}

void wrapUp(int newID) {
  if(amMoving) {
    moveResp = giveHeartbeatMove(moveCom, (millis() - moveTimer));
    amMoving = false;
    moveTimer = millis();
    communications.serialReply(moveResp);
    currentPosition = {(currentPosition.x + moveCom.magnitude), (currentPosition.y + moveCom.magnitude)};
  }
  if(amRotating) {
    rotResp = giveHeartbeatRotate(rotCom);
    amRotating = false;
    rotateTimer = millis();
    communications.serialReply(rotResp);
  }
  id = newID;
  angle = 0;
  magnitude = 0;
  instruct = "";
}

struct RotateResponse giveHeartbeatRotate(struct RotateCommand com) {
  angle = angle + 1;
  if(angle >= com.angle) {
    rotateTimer = millis();
    return (RotateResponse){id, com.angle, 1};
  }
  rotateTimer = millis();
  return (RotateResponse){id, angle, 0};
}

void rotate(struct RotateCommand com) {
  rotateTimer = millis();
}

struct ScanResponse scan(int id, float angle) {
  ScanResponse reply = {id, angle, 66.0, false};
  ray = calculations.makeLineFromPolar(reply.angle, reply.distance, currentPosition);
  equOfRay = calculations.getEquationOfLine(ray);
  nearestWall = calculations.getDestination(ray, equOfRay, PERIMETER);
  reply.distance = calculations.getDistBetweenTwoPoints(ray.x1y1, nearestWall);
  reply.last = (angleInDegrees == 357);
  return reply;
}

void makeScanCommand() {
    while(Serial.peek() != 41) {
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
    rotCom = {id, angle};
}
