#include <SPI.h>
#include <SPI_Wrapper.h>

#include <calc.h>
#include <room.h>
#include "math.h"
#include <Shared_Structs.h>
#include <QueueList.h>
#include <roomTestCases.h>

calc calculations;
roomTestCases rtc;

typedef enum {
  moveNum  = 1,
  stopNum  = 2,
  rotateNum  = 3,
  scanNum  = 4,
  compassNum = 5
} comNums;

const float SPEED = 1; //in mm per millisecond
const unsigned long SCAN_RESPONSE_INTERVAL = 100; //Values as low as 80 worked in testing
Room room;

unsigned long startedMoving, moveTimer, scanTimer, rotateTimer, distTravelled;
int magnitude, movingAngle, laserAngle, physicalAngle;
bool amScanning, amMoving, amRotating, bump;

Point terminus, endpoint, nearestWall;
command* com;
scanResponse scanResp;

float maxDegreeOfError = 100.0;
float degreeOfError = 0.0;
int angleSlip = 20;

int testCaseNum = 1;
void setup() {
  room = rtc.pickRoom(testCaseNum);
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  SPI_Wrapper::registerRotateCommandHandler(&rotateCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerCompassCommandHandler(&compassCommandHandler);
  Serial.begin(9600);
  amScanning = false;
  amMoving = false;
  amRotating = false;
  scanTimer = millis();
  rotateTimer = millis();
  Serial.println("Starting...");
}

void loop() {
  if (com != NULL) {
    if(!amMoving && !amScanning) {
      processCommand(com);
    } else if (amMoving && com->commandNumber == stopNum) {
      processCommand(com);
    }
  }

  if(amMoving && millis() >= moveTimer) {
    /*Serial.println("---------- Current Pos"); 
    Serial.print(room.robot.x);
    Serial.print(", ");
    Serial.println(room.robot.y);
    Serial.println("--------- Destination");
    Serial.print(terminus.x);
    Serial.print(", ");
    Serial.println(terminus.y);
    delay(50);*/
    respond((moveCommand*)com);
    room.robot = endpoint;
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

void rotateCommandHandler(rotateCommand rotCom) {
  if (com == NULL) {
    com =  new rotateCommand(rotCom);
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
  } else if(com->commandNumber == stopNum) {
    amMoving = false;
    unsigned long totalDistance = calculations.getDistBetweenTwoPoints(room.robot, terminus);
    distTravelled = (((millis() - startedMoving))/(moveTimer - startedMoving))*(totalDistance);
    
    // Change robot's position in room to represent the distance moved
    room.robot = calculations.makeLineFromPolar(movingAngle, distTravelled, room.robot);
    
    SPI_Wrapper::sendStopResponse(com->uniqueID, distTravelled, movingAngle, true);
  } else if(com->commandNumber == rotateNum) {
    respond((rotateCommand*)com);
    delete com;
    com = NULL;
  } else if(com->commandNumber == scanNum) {
    amScanning = true;
    scanTimer = millis() + SCAN_RESPONSE_INTERVAL;
  } else if(com->commandNumber == compassNum) {
    respond((compassCommand*)com);
    delete com;
    com = NULL;
  }
}

void respond(moveCommand* com){
  SPI_Wrapper::sendMoveResponse(com->uniqueID, distTravelled, movingAngle, bump);
}

void respond(rotateCommand* com) {
  physicalAngle = (((90 - com->angle) + 360) % 360);
  SPI_Wrapper::sendRotateResponse(com->uniqueID, com->angle, true);
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

void respond(compassCommand* com) {
  SPI_Wrapper::sendCompassResponse(com->uniqueID, (((90 - physicalAngle) + 360) % 360), true);
}

void moveRobot(moveCommand* com) {
  Point wallGuard, destination;
  amMoving = true;
  destination = calculations.makeLineFromPolar(((float)(((90 - com->angle) + 360) % 360) * PI) / 180 , com->magnitude, room.robot);
  movingAngle = com->angle + (int)lround((angleSlip * (degreeOfError / maxDegreeOfError)));
  physicalAngle = physicalAngle + (int)lround((angleSlip * (degreeOfError / maxDegreeOfError)));
  terminus = calculations.makeLineFromPolar(((float)(((90 - movingAngle) + 360) % 360) * PI) / 180, com->magnitude, room.robot);
  Line ray = Line(room.robot, terminus);
  wallGuard = terminus;
  terminus = calculations.getDestination(ray, room, amMoving);
  bump = (wallGuard.x != terminus.x || wallGuard.y != terminus.y);
  distTravelled = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, terminus) * (1.0 - (degreeOfError / maxDegreeOfError)));
  endpoint = calculations.makeLineFromPolar(((float)(((90 - movingAngle) + 360) % 360) * PI) / 180, distTravelled, room.robot);
  startedMoving = millis();
  moveTimer = millis() + calculations.getTravelTime(((com->magnitude) * 10), SPEED);
}

scanResponse scan() {
  scanResponse scanResp;
  scanResp.angle = laserAngle;
  Line ray = Line(room.robot, (calculations.makeLineFromPolar((((float)(((90 - laserAngle) + 360) % 360) * PI) / 180), 4096.0, room.robot)));
  nearestWall = calculations.getDestination(ray, room, false);
  scanResp.magnitude = (unsigned long)lround(calculations.getDistBetweenTwoPoints(ray.start, nearestWall));
  scanResp.last = (laserAngle == 360);
  return scanResp;
}

