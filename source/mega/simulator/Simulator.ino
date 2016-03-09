#include <SPI.h>
#include <SPI_Wrapper.h>

#include <comms.h>
#include <structs.h>
#include <calc.h>
#include "math.h"

calc calculations;
comms communications;

const float SPEED = 0.1; //in mm per millisecond
const int STARTING_X = 2000; //This and all distances measured in cm
const int STARTING_Y = 2000;

const MapLine MAP_BOUNDS[4] = {0, 0, 4100, 0, 4100, 0, 4100, 4100, 4100, 4100, 0, 4100, 0, 4100, 0, 0};
unsigned long startedMoving, moveTimer, rotateTimer, distTravelled;
int movingAngle;
int /*incomingByte,*/ id, magnitude, angleInDegrees;
unsigned int angle;
point currentPosition, destination, nearestWall;
//String instruct;
bool /*brakes,*/ amMoving, amRotating;
MapLine ray;
EquationOfLine PERIMETER[4];
EquationOfLine equOfRay;

command* com;
//MoveCommand moveCom;
//MoveResponse moveResp = {0, 0, 0, 0};

//RotateCommand rotCom = {0, 0};
//RotateResponse rotResp = {0, 0, 0};

ScanResponse scanResp = {0, 0, 0, false, false};

void setup() {
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  Serial.begin(9600);
  //instruct = "";
  //incomingByte = 0;
  id = 0;
  angle = 0;
  angleInDegrees = 0;
  magnitude = 0;
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  //brakes = false;
  amMoving = false;
  amRotating = false;
  for(int i = 0; i < 4; i++) {
    PERIMETER[i] = calculations.getEquationOfLine(MAP_BOUNDS[i]);
  }
}

void loop() {
  
  /*if(Serial.available() > 0) {
    instruct += communications.parseCommand();
    delay(10); //Required to allow read buffer to recover
  }*/

  if (com != NULL) {
    if(!amMoving && !amRotating) {
      processCommand(com);
      //Reset global variables
      //id = 0;
      //angle = 0;
      //magnitude = 0;
    } else if (amMoving && com->commandNumber == 2) {
      processCommand(com);
    }
  }

  if(amMoving && millis() >= moveTimer) {
    respond((moveCommand*)com);
    currentPosition = destination;
    amMoving = false;
    delete com;
    com = NULL;
  }

  if(amRotating && angle <= 30) {
    scanResp = scan((scanCommand*)com);
    respond(scanResp);
    angle++;
  }
  
  if(amRotating && angle > 30) {
    amRotating = false;
    angle = 0;
    delete com;
    com = NULL;
  }
  
  /*if(instruct == "rotate(") {
    //rotCom = communications.constructRotateCommand();
    makeRotateCommand();
    rotate(rotCom);
    id = 0;
    angle = 0;
  }
  if(instruct == "scan(") {
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
  }
  if(instruct == "stopRobot();") {
    stopRobot(); //Currently non-functional
  }
  if(brakes) {
    wrapUp(1);
    brakes = false;
  }*/
}

/*void makeMoveCommand() {
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
    //angle = (angle * PI) / 180.0;
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      magnitude = magnitude * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
    //moveCom = {id, angle, magnitude};
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
}*/

void moveCommandHandler(moveCommand movCom) {
  //Serial.println("Received move command over SPI");
  //respond(movCom);
  moveCommand* temp = new moveCommand(movCom);
  com = temp;
}

void stopCommandHandler(stopCommand stopCom) {
  com = new stopCommand(stopCom);
}

void scanCommandHandler(scanCommand scanCom) {
  scanCommand* temp = new scanCommand(scanCom);
  com = temp;
}

void processCommand(command* com) {
  if(com->commandNumber == 1){
    moveRobot((moveCommand*)com);
  }
  else if(com->commandNumber == 2){
    amMoving = false;
    unsigned long totalDistance = calculations.getDistBetweenTwoPoints(currentPosition, destination);
    float distanceMoved = (((float)(millis() - startedMoving))/(float)(moveTimer - startedMoving))*(float)(totalDistance);
    
    // Change currentPosition to represent the distance moved
    ray = calculations.makeLineFromPolar(movingAngle, (uint16_t)distanceMoved, currentPosition);
    equOfRay = calculations.getEquationOfLine(ray);
    currentPosition = calculations.getDestination(ray, equOfRay, PERIMETER);
    
    SPI_Wrapper::sendStopResponse(com->uniqueID, (uint16_t)distanceMoved, (uint16_t)movingAngle, true);
  }
  else if(com->commandNumber == 3){
    // rotate command to be implemented
  }
  else if(com->commandNumber == 4){
    amRotating = true;
  }
}
void respond(moveCommand* com){
  SPI_Wrapper::sendMoveResponse(com->uniqueID, com->magnitude, com->angle, true);
}

void respond(ScanResponse scanResp) {
  SPI_Wrapper::sendScanResponse(scanResp.uniqueID, scanResp.angle, scanResp.distance, scanResp.last, scanResp.status);
  Serial.println("Response sending function invoked successfully!");
  communications.serialReply(scanResp);
}

void moveRobot(moveCommand* com) {
  ray = calculations.makeLineFromPolar(com->angle, com->magnitude, currentPosition);
  equOfRay = calculations.getEquationOfLine(ray);
  destination = calculations.getDestination(ray, equOfRay, PERIMETER);
  com->magnitude = (unsigned long)calculations.getDistBetweenTwoPoints(equOfRay.xy, destination);
  amMoving = true;
  movingAngle = com->angle;
  startedMoving = millis();
  moveTimer = millis() + calculations.getTravelTime(((com->magnitude) * 10), SPEED);
}

ScanResponse scan(scanCommand* com){
  ScanResponse scanResp = {com->uniqueID, angle, 65530, false, false};
  ray = calculations.makeLineFromPolar(((((float)angle) * PI) / 180), (float)scanResp.distance, currentPosition);
  equOfRay = calculations.getEquationOfLine(ray);
  nearestWall = calculations.getDestination(ray, equOfRay, PERIMETER);
  scanResp.distance = (unsigned int)calculations.getDistBetweenTwoPoints(ray.x1y1, nearestWall);
  scanResp.last = (angle == 360);
  return scanResp;
}

/*struct MoveResponse giveHeartbeatMove(struct MoveCommand com, unsigned long time) {
  distTravelled = distTravelled + ((SPEED * time) * 10);
  if(distTravelled >= com.magnitude) {
    amMoving = false;
    moveTimer = millis();
    return (MoveResponse){com.uniqueID, com.angle, com.magnitude, 1};
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
}*/
