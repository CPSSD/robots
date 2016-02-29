#include <LIDARLite.h>

#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>
#include <I2C_Wrapper.h>

String STATE = "WAITING";
int detectionAngle = 0;
boolean waitingToScan = false;

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

// Set the state to SCAN until a scan is complete.
void scanCommandHandler(scanCommand command) {
  Serial.println("Recieved Scan Command");
  if (STATE == "WAITING") {
    STATE = "SCAN"
    scanArea(3360/300, motor.singleRotation);
    STATE = "WAITING"
  } else if (STATE == "DETECT") {
    Serial.println("Waiting for detection to stop before scanning.");
    STATE = "SCAN"
    waitingToScan = true;
  } else {
    Serial.println("Unable to start scan. Invalid STATE");
  }
}

// Set the state to DETECT (IFF state is WAITING)
void detectionCommandHandler(detectCommand command){
  if (STATE == "WAITING") {
    STATE = "DETECT"
    detectObjects(detectionAngle, 1);
  } else {
    Serial.println("Unable to change state to DETECT. Reason: Scan command in progress");
  }
}

// Set the current detection angle to the move angle.
void moveCommandHandler(moveCommand command){
  Serial.print("Changing detection angle to ")
  Serial.println(command.angle);
  detectionAngle = command.angle;
}

void setup() {
  Serial.begin(9600);
  
  I2C_Wrapper::init(Slave, 27);
  I2C_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  I2C_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  I2C_Wrapper::registerDetectionCommandHandler(&detectionCommandHandler);
  
  motor.setup();
  scanner.setup();
}

void loop() {
  Serial.println("Starting LaserMotorArm");
  //faceWall(3360/180, motor.singleRotation, 210, false);
  //faceWall(10, 420, 0, true);
  Serial.println("*Laser now faces wall");

  //detectObjects(0, 2);
  //scanArea(3360/70, motor.singleRotation);
  
  Serial.println("Starting main loop...");
  while(1) {
    if (STATE == "DETECT") {
      detectObjects(detectionAngle, 1);
    } else if (STATE == "SCAN" && waitingToScan) {
      waitingToScan = false;
      scanArea(3360/70, motor.singleRotation);
      STATE = "WAITING";
    }
  }
}
