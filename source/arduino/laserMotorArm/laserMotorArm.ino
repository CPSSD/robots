#include <LIDARLite.h>

#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>

Motor motor = Motor();
LaserScanner scanner = LaserScanner();
const boolean DEBUG = false;

void faceWallAttempt2(int scanFreq, int distance, int finishOffset, bool returnToStart){
  motor.setSpeed(200);
  LaserScanner::setScanFreq(scanFreq);
  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  LaserReading goal = LaserReading{-1, -1};
  LaserScanner::reset();

  for (int i = 0; i < 42; i++){
    LaserScanner::lastRotationData[i] = {-1, -1};
  }

  int offset = motor.rotateWithCorrection(distance);
  motor.encoderCount = 0;

  long ticksToTravel = 0;

  for (int i = 0; i < 42; i++){

    LaserReading reading = LaserScanner::lastRotationData[i];
    if (reading.distance != -1 && ((goal.distance == -1)  || (reading.distance < goal.distance))){
      goal = LaserScanner::lastRotationData[i];
    }

    ticksToTravel = ((long)(distance /42)) * goal.angle;

    if (DEBUG){
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
 if (returnToStart) {
    motor.changeDirection();
    ticksToTravel = distance-ticksToTravel;
 }

 int nextDistance = ticksToTravel + offset - finishOffset;
 if (nextDistance < 0){
  nextDistance = 3360-nextDistance;
 }
 motor.rotateWithCorrection(nextDistance);

 if (returnToStart) {
  motor.changeDirection();
 }
  
}

void detectObjects(int angle){
  boolean detectingObjects = true;
  int rotationTick = 0;
  
  motor.setSpeed(200);
  motor.registerRotationFunction(&LaserScanner::detectObjects);
  LaserScanner::setDetectionAngle(angle);
  LaserScanner::setDetectionRange(25);
  
  while(detectingObjects){
    rotationTick++;
    motor.rotateWithCorrection(motor.singleRotation);

    if (rotationTick == 5){
      detectingObjects = false;
    }
  
  }
}

void setup(){
    motor.setup();
    scanner.setup();
    Serial.begin(9600);
}

void loop(){
    faceWallAttempt2(80, motor.singleRotation, 210, false);
    faceWallAttempt2(10, 420, 0, true);
    Serial.println("*Laser now faces wall");
    
    detectObjects(0);
    
    int fullSpin = motor.singleRotation;
    
    //motor.rotateWithCorrection(fullSpin);
    //motor.rotateByAngle(360);
  
    while(1);
}
