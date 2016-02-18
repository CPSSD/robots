#include <LIDARLite.h>

#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>
#include "SPI_Wrapper.h"

Motor motor = Motor();
LaserScanner scanner = LaserScanner();
const boolean DEBUG = false;
int totalScans = 0;

void faceWall(int scanFreq, int distance, int finishOffset, bool isPartialMove) {
  motor.setSpeed(150);
  LaserScanner::setScanFreq(scanFreq, distance);
  totalScans = LaserScanner::scansToDo;
  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  LaserReading goal = LaserReading{ -1, -1};
  LaserScanner::reset();

  for (int i = 0; i < totalScans; i++) {
    LaserScanner::lastRotationData[i] = { -1, -1};
  }

  int offset = motor.rotateWithCorrection(distance);
  motor.encoderCount = 0;

  long ticksToTravel = 0;

  for (int i = 0; i < totalScans; i++) {

    LaserReading reading = LaserScanner::lastRotationData[i];
    if (reading.distance != -1 && ((goal.distance == -1)  || (reading.distance < goal.distance))) {
      goal = LaserScanner::lastRotationData[i];
    }

    ticksToTravel = ((long)(distance / totalScans)) * goal.angle;

    if (DEBUG) {
      Serial.print(i);
      Serial.print(". Current Smallest Measurement: ");
      Serial.println(goal.distance);
      Serial.print("    Distance to Travel: ");
      Serial.println(ticksToTravel);
      Serial.print("    Angle: ");
      Serial.println(goal.angle);
      Serial.print("    Distance: ");
      Serial.println(distance);
    }
  }

  motor.setSpeed(150);
  if (isPartialMove) {
    motor.changeDirection();
    ticksToTravel = distance - ticksToTravel;
  }

  int nextDistance = ticksToTravel + offset - finishOffset;
  if (nextDistance < 0) {
    nextDistance = 3360 - nextDistance;
  }
  motor.rotateWithCorrection(nextDistance);

  if (isPartialMove) {
    motor.changeDirection();
  }

}

void detectObjects(int angle, int rotations) {
  boolean detectingObjects = true;
  int rotationTick = 0;

  motor.setSpeed(200);
  motor.registerRotationFunction(&LaserScanner::detectObjects);
  LaserScanner::setDetectionAngle(angle);
  LaserScanner::setDetectionRange(25);

  motor.rotateContinuous(rotations);
  Serial.println("Finished Rotating Continuously");
}

void scanArea(int scanFreq, int distance){
  motor.setSpeed(150);
  Serial.println("Scanning Area...");
  
  LaserScanner::setScanFreq(scanFreq, distance);
  totalScans = LaserScanner::scansToDo;
  LaserScanner::reset();
  
  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  LaserScanner::pushScanData = true;
  motor.rotateWithCorrection(distance);

  Serial.println("Finished sending data...");
}

void setup() {
  SPI_Wrapper::init();
  motor.setup();
  scanner.setup();
  Serial.begin(9600);
}

void loop() {
  Serial.println("Starting LaserMotorArm");
  //faceWall(3360/180, motor.singleRotation, 210, false);
  //faceWall(10, 420, 0, true);
  Serial.println("*Laser now faces wall");

  //detectObjects(0, 2);

  scanArea(3360/70, motor.singleRotation);

  while (1);
}
