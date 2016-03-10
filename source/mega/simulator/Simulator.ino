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
int id, magnitude, movingAngle, laserAngle;
point currentPosition, destination, nearestWall;
bool amScanning, amMoving, amRotating;
MapLine ray;
EquationOfLine PERIMETER[4];
EquationOfLine equOfRay;

command* com;

ScanResponse scanResp = {0, 0, 0, false, false};

void setup() {
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerStopCommandHandler(&stopCommandHandler);
  Serial.begin(9600);
  id = 0;
  laserAngle = 0;
  movingAngle = 0;
  magnitude = 0;
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
    else if (amMoving && com->commandNumber == 2) {
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

  if(amScanning && laserAngle <= 30) {
    scanResp = scan((scanCommand*)com);
    respond(scanResp);
    laserAngle++;
  }
  
  if(amScanning && laserAngle > 30) {
    amScanning = false;
    laserAngle = 0;
    delete com;
    com = NULL;
  }
}

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
    amScanning = true;
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
  ScanResponse scanResp = {com->uniqueID, laserAngle, 65530, false, false};
  ray = calculations.makeLineFromPolar(((((float)laserAngle) * PI) / 180), (float)scanResp.distance, currentPosition);
  equOfRay = calculations.getEquationOfLine(ray);
  nearestWall = calculations.getDestination(ray, equOfRay, PERIMETER);
  scanResp.distance = (unsigned int)calculations.getDistBetweenTwoPoints(ray.x1y1, nearestWall);
  scanResp.last = (laserAngle == 360);
  return scanResp;
}

