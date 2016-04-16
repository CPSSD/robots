#include <SPI.h>
#include <SPI_Wrapper.h>

#include <calc.h>
#include <room.h>
#include "math.h"
#include <Shared_Structs.h>
#include <QueueList.h>

calc calculations;

typedef enum {
  moveNum  = 1,
  stopNum  = 2,
  rotateNum  = 3,
  scanNum  = 4,
  compassNum = 5
} comNums;

const float SPEED = 1; //in mm per millisecond
const int STARTING_X = 30; //This and all distances measured in cm
const int STARTING_Y = 30;
const unsigned long SCAN_RESPONSE_INTERVAL = 100; //Values as low as 80 worked in testing
Point MAP_BOUNDS[] = { {Point(0, 0)}, {Point(300, 0)}, {Point(300, 300)}, {Point(0, 300)} };
Room room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 1);

unsigned long startedMoving, moveTimer, scanTimer, rotateTimer, distTravelled;
int magnitude, movingAngle, laserAngle, physicalAngle;
bool amScanning, amMoving, amRotating;

Point currentPosition, destination, terminus, nearestWall;
command* com;
scanResponse scanResp;

float maxDegreeOfError = 100.0;
float degreeOfError = 0.0;
int angleSlip = 20;

void setup() {
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  SPI_Wrapper::registerCompassCommandHandler(&compassCommandHandler);
  Serial.begin(9600);
  Serial.println("Starting...");
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  amScanning = false;
  amMoving = false;
  amRotating = false;
  Point newObjHolder[] = { {Point(145, 145)}, {Point(155, 145)}, {Point(155, 155)}, {Point(145, 155)} };
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
    respond((moveCommand*)com);
    Serial.println("---------- Current Pos"); 
    Serial.print(currentPosition.x);
    Serial.println(", ");
    Serial.println(currentPosition.y);
    Serial.println("--------- Destination");
    Serial.print(destination.x);
    Serial.println(", ");
    Serial.println(destination.y);
    currentPosition = destination;
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

void compassCommandHandler(compassCommand compCom) {
  if (com == NULL) {
    com =  new compassCommand(compCom);
  }
}

void processCommand(command* com) {
  if(com->commandNumber == moveNum) {
    moveRobot((moveCommand*)com);
  }
  else if(com->commandNumber == stopNum) {
    amMoving = false;
    unsigned long totalDistance = calculations.getDistBetweenTwoPoints(currentPosition, destination);
    float distanceMoved = (((float)(millis() - startedMoving))/(float)(moveTimer - startedMoving))*(float)(totalDistance);
    
    // Change currentPosition to represent the distance moved
    currentPosition = calculations.makeLineFromPolar(movingAngle, distanceMoved, currentPosition);
    
    SPI_Wrapper::sendStopResponse(com->uniqueID, distanceMoved, movingAngle, true);
  }
  else if(com->commandNumber == rotateNum){
    // rotate command to be implemented
  }
  else if(com->commandNumber == scanNum) {
    amScanning = true;
    scanTimer = millis() + SCAN_RESPONSE_INTERVAL;
  }
  else if(com->commandNumber == compassNum) {
    respond((compassCommand*)com);
    delete com;
    com = NULL;
  }
}

void respond(moveCommand* com) {
  SPI_Wrapper::sendMoveResponse(com->uniqueID, distTravelled, movingAngle, true);
}

void respond(scanResponse scanResp) {
  Serial.println("Sending Scan Response...");
  SPI_Wrapper::sendScanResponse(com->uniqueID, scanResp.angle, scanResp.magnitude, scanResp.last, true);
}

void respond(compassCommand* com) {
  SPI_Wrapper::sendCompassResponse(com->uniqueID, ((physicalAngle - 90) % 360), true);
}

void moveRobot(moveCommand* com) {
  destination = calculations.makeLineFromPolar((float((com->angle + 90) % 360) * PI) / 180 , com->magnitude, currentPosition);
  movingAngle = com->angle + (int)lround((angleSlip * (degreeOfError / maxDegreeOfError)));
  physicalAngle = physicalAngle + (int)lround((angleSlip * (degreeOfError / maxDegreeOfError)));
  terminus = calculations.makeLineFromPolar((float((movingAngle + 90) % 360) * PI) / 180, com->magnitude, currentPosition);
  Line ray = Line(currentPosition, terminus);
  terminus = calculations.getDestination(ray, room);
  distTravelled = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, terminus) * (1.0 - (degreeOfError / maxDegreeOfError)));
  amMoving = true;
  startedMoving = millis();
  moveTimer = millis() + calculations.getTravelTime(((com->magnitude) * 10), SPEED);
}

scanResponse scan() {
  scanResponse scanResp;
  scanResp.angle = laserAngle;
  Line ray = Line(currentPosition, (calculations.makeLineFromPolar((((float)((laserAngle + 90) % 360) * PI) / 180), 4096.0, currentPosition)));
  nearestWall = calculations.getDestination(ray, room);
  scanResp.magnitude = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, nearestWall));
  scanResp.last = (laserAngle == 360);
  return scanResp;
}

