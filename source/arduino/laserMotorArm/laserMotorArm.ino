#include <LIDARLite.h>

#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>
#include <I2C_Wrapper.h>

String STATE = "WAITING";
int detectionAngle = 0;
boolean waitingToScan = false;

const int totalRotations = 2;
const int motorSpeed = 150;

Motor motor = Motor();
LaserScanner scanner = LaserScanner();
const boolean DEBUG = false;
int totalScans = 0;

void faceWall(int scanFreq, int distance, int finishOffset, bool isPartialMove) {
  motor.setSpeed(motorSpeed);
  LaserScanner::setScanFreq(scanFreq, distance, "DEFAULT", 1);
  totalScans = LaserScanner::scansToDo;
  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  motor.registerRotationFinishedFunction(&LaserScanner::onMotorFinish);
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

  motor.setSpeed(motorSpeed);
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
  motor.registerRotationFinishedFunction(&LaserScanner::onMotorFinish);
  LaserScanner::setDetectionAngle(angle);
  LaserScanner::setDetectionRange(25);

  motor.rotateContinuous(rotations);
  Serial.println("Finished Rotating Continuously");
}

void scanArea(int scanFreq, int distance, String scanType) {
  motor.setSpeed(motorSpeed);
  Serial.println("Scanning Area...");

  LaserScanner::setScanFreq(scanFreq, distance, scanType, totalRotations);
  totalScans = LaserScanner::scansToDo;

  LaserScanner::reset();

  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  motor.registerRotationFinishedFunction(&LaserScanner::onMotorFinish);
  if (scanType == "AVERAGE") {
    for (int i = 0; i < totalRotations; i++) {
      LaserScanner::reset();
      LaserScanner::pushScanData = true;
      motor.rotateWithCorrection(distance);
    }
    LaserScanner::totalRotations = 0;

    for (int i = 0; i < LaserScanner::scansToDo; i++) {
      if (LaserScanner::lastRotationData[i].angle / totalRotations != i) {
        Serial.println("\t- Error @ this location");
      } else {
        if (LaserScanner::pushScanData) {
          Serial.println("\t- Sending Scan Data");
          LaserScanner::sendScanResponse(LaserReading{LaserScanner::lastRotationData[i].angle / LaserScanner::queuedRotations, LaserScanner::lastRotationData[i].distance / LaserScanner::queuedRotations});
        }
      }
      Serial.print("[");
      Serial.print("i");
      Serial.print("]: ");
      Serial.print(LaserScanner::lastRotationData[i].angle / totalRotations);
      Serial.print(", ");
      Serial.println(LaserScanner::lastRotationData[i].distance / totalRotations);
    }
  } else if (scanType == "DEFAULT") {
    LaserScanner::pushScanData = true;
    motor.rotateWithCorrection(distance);
  } else if (scanType == "INTERVAL") {
    for (int i = 0; i < totalRotations; i++) {
      LaserScanner::reset();
      LaserScanner::pushScanData = true;
      LaserScanner::setScanOffset(i);
      motor.rotateWithCorrection(distance);
    }

    LaserScanner::reset();
  } else {
    Serial.print("Unknown scan type: ");
    Serial.println(scanType);
  }

  Serial.println("Finished sending data...");
}

// Set the state to SCAN until a scan is complete.
void scanCommandHandler(scanCommand command) {
  Serial.println("Recieved Scan Command");
  if (STATE == "WAITING" || STATE == "DETECT") {
    STATE = "SCAN";
    waitingToScan = true;
  } else {
    Serial.println("Unable to start scan. Invalid STATE or already scanning.");
  }
}

// Set the state to DETECT (IFF state is WAITING)
void detectCommandHandler(detectCommand command) {
  if (STATE == "WAITING") {
    STATE = "DETECT";
  } else {
    Serial.println("Unable to change state to DETECT. Reason: Scan command in progress");
  }
}

// Set the current detection angle to the move angle.
void moveCommandHandler(moveCommand command) {
  Serial.print("Changing detection angle to ");
  Serial.println(command.angle);
}

void setup() {
  Serial.begin(9600);

  I2C_Wrapper::init(Slave, 27);
  I2C_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  I2C_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  I2C_Wrapper::registerDetectCommandHandler(&detectCommandHandler);

  motor.setup();
  scanner.setup();
}

void loop() {
  Serial.println("Starting LaserMotorArm");
  //faceWall(3360/180, motor.singleRotation, 210, false);
  //faceWall(10, 420, 0, true);
  Serial.println("*Laser now faces wall");

  STATE = "SCAN";
  waitingToScan = true;

  Serial.println("Starting main loop...");
  while (1) {
    if (STATE == "DETECT") {
      detectObjects(detectionAngle, 1);
    } else if (STATE == "SCAN" && waitingToScan) {
      waitingToScan = false;
      scanArea(3360 / 300, motor.singleRotation, "AVERAGE");
      STATE = "WAITING";
    }
  }
}
