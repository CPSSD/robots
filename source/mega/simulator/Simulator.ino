#include <SPI.h>
#include <SPI_Wrapper.h>

#include <structs.h>
#include <calc.h>
#include "math.h"
#include <Shared_Structs.h>

calc calculations;

const float SPEED = 0.1; //in mm per millisecond
const int STARTING_X = 150; //This and all distances measured in cm
const int STARTING_Y = 150;
const MapLine MAP_BOUNDS[4] = { {0, 0, 300, 0}, {300, 0, 300, 300}, {300, 300, 0, 300}, {0, 300, 0, 0} };

unsigned long startedMoving, moveTimer, rotateTimer, distTravelled;
int id, magnitude, movingAngle, laserAngle;
bool amScanning, amMoving, amRotating;

point currentPosition, destination, nearestWall;
EquationOfLine PERIMETER[4];
command* com;
scanResponse scanResp;

void setup() {
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  Serial.begin(9600);
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  amScanning = false;
  amMoving = false;
  amRotating = false;
  for(int i = 0; i < 4; i++) {
    PERIMETER[i] = calculations.getEquationOfLine(MAP_BOUNDS[i]);
  }
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
    currentPosition = destination;
    amMoving = false;
    delete com;
    com = NULL;
  }

  if(amScanning && laserAngle <= 360) {
    scanResp = scan();
    respond(scanResp);
    laserAngle++;
  }
  
  if(amScanning && laserAngle > 360) {
    amScanning = false;
    laserAngle = 0;
    delete com;
    com = NULL;
  }
}

void moveCommandHandler(moveCommand movCom) {
  com = new moveCommand(movCom);
}

void stopCommandHandler(stopCommand stopCom) {
  com = new stopCommand(stopCom);
}

void scanCommandHandler(scanCommand scanCom) {
  com =  new scanCommand(scanCom);
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
    MapLine ray = calculations.makeLineFromPolar(movingAngle, (uint16_t)distanceMoved, currentPosition);
    EquationOfLine equOfRay = calculations.getEquationOfLine(ray);
    currentPosition = calculations.getDestination(ray, equOfRay, PERIMETER);
    
    SPI_Wrapper::sendStopResponse(com->uniqueID, (uint16_t)distanceMoved, (uint16_t)movingAngle, true);
  }
  else if(com->commandNumber == rotateNum){
    // rotate command to be implemented
  }
  else if(com->commandNumber == scanNum){
    amScanning = true;
  }
}

void respond(moveCommand* com){
  SPI_Wrapper::sendMoveResponse(com->uniqueID, com->magnitude, com->angle, true);
}

void respond(scanResponse scanResp) {
  SPI_Wrapper::sendScanResponse(com->uniqueID, scanResp.angle, scanResp.magnitude, scanResp.last, true);
}

void moveRobot(moveCommand* com) {
  MapLine ray = calculations.makeLineFromPolar(com->angle, com->magnitude, currentPosition);
  EquationOfLine equOfRay = calculations.getEquationOfLine(ray);
  destination = calculations.getDestination(ray, equOfRay, PERIMETER);
  com->magnitude = (unsigned long)round(calculations.getDistBetweenTwoPoints(equOfRay.xy, destination));
  amMoving = true;
  movingAngle = com->angle;
  startedMoving = millis();
  moveTimer = millis() + calculations.getTravelTime(((com->magnitude) * 10), SPEED);
}

scanResponse scan(){
  scanResponse scanResp;
  scanResp.angle = laserAngle;
  MapLine ray = calculations.makeLineFromPolar(((((float)laserAngle) * PI) / 180), 65530.0, currentPosition);
  EquationOfLine equOfRay = calculations.getEquationOfLine(ray);
  nearestWall = calculations.getDestination(ray, equOfRay, PERIMETER);
  scanResp.magnitude = (unsigned int)round(calculations.getDistBetweenTwoPoints(ray.x1y1, nearestWall));
  scanResp.last = (laserAngle == 360);
  return scanResp;
}

