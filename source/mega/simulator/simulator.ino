#include <QueueList.h>
#include <SPI.h>
#include <SPI_Wrapper.h>

#include <calc.h>
#include <room.h>
#include "math.h"
#include <Shared_Structs.h>

calc calculations;

typedef enum {
  moveNum  = 1,
  stopNum  = 2,
  rotateNum  = 3,
  scanNum  = 4
} comNums;

const float SPEED = 1; //in mm per millisecond
const int STARTING_X = 200; //This and all distances measured in cm
const int STARTING_Y = 200;
const unsigned long SCAN_RESPONSE_INTERVAL = 100; //Values as low as 80 worked in testing
Point MAP_BOUNDS[] = { {Point(0, 0)}, {Point(600, 0)}, {Point(600, 600)}, {Point(0, 600)} };
Room room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 1);

unsigned long startedMoving, moveTimer, scanTimer, rotateTimer, distTravelled;
int id, magnitude, movingAngle, laserAngle;
bool amScanning, amMoving, amRotating;

Point currentPosition, destination, terminus, nearestWall;
command* com;
scanResponse scanResp;

float maxDegreeOfError = 100.0;
float degreeOfError = 0.0;
int angleSlip = 3;

void setup() {
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  Serial.begin(9600);
  Serial.println("Starting...");
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  amScanning = false;
  amMoving = false;
  amRotating = false;
  Point newObjHolder[] = { {Point(260, 260)}, {Point(340, 260)}, {Point(340, 340)}, {Point(260, 340)} };
  Serial.print("Adding object 1: ");
  Serial.println(room.addObject(0, 4, newObjHolder));
  scanTimer = millis();
}

void loop() {
  if (com != NULL) {
    if(!amMoving && !amScanning) {
      processCommand(com);
    }
    else if (amMoving && com->commandNumber == stopNum) {
      processCommand(com);
    }
  }

  if(amMoving && millis() >= moveTimer) {
    /*Serial.println("---------- Current Pos"); 
    Serial.print(currentPosition.x);
    Serial.print(", ");
    Serial.println(currentPosition.y);
    Serial.println("--------- Destination");
    Serial.print(destination.x);
    Serial.print(", ");
    Serial.println(destination.y);
    delay(50);*/
    respond((moveCommand*)com);
    currentPosition = terminus;
    amMoving = false;
    delete com;
    com = NULL;
  }

  if(amScanning && laserAngle <= 360 && millis() >= scanTimer) {
    scanResp = scan();
    if(scanResp.magnitude != 4096) {
      respond(scanResp);
    }
    laserAngle += 5;
    scanTimer = millis() + SCAN_RESPONSE_INTERVAL;
  }
  
  if(amScanning && laserAngle > 360) {
    amScanning = false;
    laserAngle = 0;
    delete com;
    com = NULL;
  }
}

void moveCommandHandler(moveCommand movCom) {
  Serial.println("Recieved Move Command...");
  if (com == NULL) {
    com = new moveCommand(movCom);
  }
}

void stopCommandHandler(stopCommand stopCom) {
  Serial.println("Recieved Stop Command...");
  if (amMoving) {
    if (com != NULL) {
      delete com;
    }
    com = new stopCommand(stopCom);
  }
}

void scanCommandHandler(scanCommand scanCom) {
  Serial.println("Recieved Scan Command...");
  if (com == NULL) {
    com =  new scanCommand(scanCom);
  }
}

void processCommand(command* com) {
  if(com->commandNumber == moveNum){
    moveRobot((moveCommand*)com);
  }
  else if(com->commandNumber == stopNum){
    amMoving = false;
    unsigned long totalDistance = calculations.getDistBetweenTwoPoints(currentPosition, destination);
    float distanceMoved = (((float)(millis() - startedMoving))/(float)(moveTimer - startedMoving))*(float)(totalDistance);
    
    // Change currentPosition to represent the distance moved
    currentPosition = calculations.makeLineFromPolar(movingAngle, (uint16_t)distanceMoved, currentPosition);
    
    SPI_Wrapper::sendStopResponse(com->uniqueID, (uint16_t)distanceMoved, (uint16_t)movingAngle, true);
  }
  else if(com->commandNumber == rotateNum){
    // rotate command to be implemented
  }
  else if(com->commandNumber == scanNum){
    amScanning = true;
    scanTimer = millis() + SCAN_RESPONSE_INTERVAL;
  }
}

void respond(moveCommand* com){
  SPI_Wrapper::sendMoveResponse(com->uniqueID, distTravelled, movingAngle, (distTravelled == com->magnitude));
}

void respond(scanResponse scanResp) {
  Serial.println("Sending Scan Response...");
  /*Serial.print("Angle: ");
  Serial.print(scanResp.angle);
  Serial.print("\t Distance: ");
  Serial.print(scanResp.magnitude);
  Serial.println();*/
  SPI_Wrapper::sendScanResponse(com->uniqueID, scanResp.angle, scanResp.magnitude, scanResp.last, true);
}

void moveRobot(moveCommand* com) {
  destination = calculations.makeLineFromPolar((float((com->angle + 90) % 360) * PI) / 180 , com->magnitude, currentPosition);
  movingAngle = com->angle + (int)lround((angleSlip * (degreeOfError / maxDegreeOfError)));
  terminus = calculations.makeLineFromPolar((float((movingAngle + 90) % 360) * PI) / 180, com->magnitude, currentPosition);
  Line ray = Line(currentPosition, terminus);
  terminus = calculations.getDestination(ray, room);
  distTravelled = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, terminus) * (1.0 - (degreeOfError / maxDegreeOfError)));
  amMoving = true;
  startedMoving = millis();
  moveTimer = millis() + calculations.getTravelTime(((com->magnitude) * 10), SPEED);
}

scanResponse scan(){
  scanResponse scanResp;
  scanResp.angle = laserAngle;
  com->uniqueID = 4;
  Line ray = Line(currentPosition, (calculations.makeLineFromPolar((((float)((laserAngle + 90) % 360) * PI) / 180), 4096.0, currentPosition)));
  nearestWall = calculations.getDestination(ray, room);
  scanResp.magnitude = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, nearestWall));
  scanResp.last = (laserAngle == 360);
  return scanResp;
}

