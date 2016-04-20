#include <LIDARLite.h>
#include <Shared_Structs.h>
#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>
#include <I2C_Wrapper.h>

enum LaserState {
  Waiting,
  Scanning,
  Detecting
};

const int slaveID = 25;

LaserState STATE = Waiting;
int detectionAngle = 0;
boolean waitingToScan = false;

int detectEnd, detectBegin, detectDistance;
const int totalRotations = 2;
const int motorSpeed = 150;

Motor motor = Motor();
LaserScanner scanner = LaserScanner();
const boolean DEBUG = false;
int totalScans = 0;

void faceWall(int scanFreq, int distance, int finishOffset, bool isPartialMove) {
  motor.setSpeed(motorSpeed);
  LaserScanner::setScanFreq(scanFreq, distance, Default, 1);
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

int degreesToEncoderCount(int angle){
 return ((float) angle / 360.0) * 3360.0;
}

void detectObjects(int startAngle, int endAngle, int distance) {
  boolean detectingObjects = true;

  motor.setSpeed(200);
  motor.registerRotationFunction(&LaserScanner::detectObjects);
  motor.registerRotationFinishedFunction(&LaserScanner::onMotorFinish);
  LaserScanner::setDetectionParameters(startAngle, endAngle, distance);

  motor.rotateWithCorrection(3360);
  //Serial.println("Finished Rotating Continuously");
  LaserScanner::reset();
  delay(1000);
}

void scanArea(int scanFreq, int distance, ScanType scanType) {
  motor.setSpeed(motorSpeed);
  //Serial.println("Scanning Area...");

  LaserScanner::setScanFreq(scanFreq, distance, scanType, totalRotations);
  totalScans = LaserScanner::scansToDo;

  LaserScanner::reset();

  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  motor.registerRotationFinishedFunction(&LaserScanner::onMotorFinish);
  LaserScanner::totalRotations = 0;
  if (scanType == Average) {
    for (int i = 0; i < totalRotations; i++) {
      LaserScanner::reset();
      LaserScanner::pushScanData = true;
      motor.rotateWithCorrection(distance);
    }
    LaserScanner::totalRotations = 0;

    
    LaserReading firstReading;
    for (int i = 0; i < LaserScanner::scansToDo; i++) {
      if (LaserScanner::lastRotationData[i].angle / totalRotations != i) {
        //Serial.println("\t- Error @ this location");
      } else {
        firstReading = LaserScanner::lastRotationData[i];
        if (LaserScanner::pushScanData) {
          //Serial.println("\t- Sending Scan Data");
          delay(40);
          LaserScanner::sendScanResponse(LaserReading{LaserScanner::lastRotationData[i].angle / LaserScanner::queuedRotations, LaserScanner::lastRotationData[i].distance / LaserScanner::queuedRotations}, false);
        }
      }
    }  
    LaserScanner::sendScanResponse(LaserReading{firstReading.angle / LaserScanner::queuedRotations, firstReading.distance / LaserScanner::queuedRotations}, true);    
  } else if (scanType == Default) {
    LaserScanner::pushScanData = true;
    motor.rotateWithCorrection(distance);
  } else if (scanType == Interval) {
    for (int i = 0; i < totalRotations; i++) {
      LaserScanner::reset();
      LaserScanner::pushScanData = true;
      LaserScanner::setScanOffset(i);
      motor.rotateWithCorrection(distance);
    }

    LaserScanner::reset();
  }

  //Serial.println("Finished sending data...");
}

// Set the state to SCAN until a scan is complete.
void scanCommandHandler(scanCommand command) {
  Serial.println("Recieved Scan Command");
  if (STATE == Waiting || STATE == Detecting) {
    STATE = Scanning;
    waitingToScan = true;
  } else {
    //Serial.println("Unable to start scan. Invalid STATE or already scanning.");
  }
}

// Set the state to DETECT (IFF state is WAITING)
void detectCommandHandler(detectCommand command) {
  if (STATE == Waiting) {
    detectBegin = degreesToEncoderCount(command.rangeBegin);
    detectEnd = degreesToEncoderCount(command.rangeEnd);
    detectDistance = command.maxDistance;
    //Serial.println("[Detection Command Recieved]");
    //Serial.print("Start: ");
    //Serial.println(detectBegin);
    //Serial.print("End: ");
    //Serial.println(detectEnd);
    //Serial.print("Distance: ");
    //Serial.println(detectDistance);
    STATE = Detecting;
  } else {
    //Serial.println("Unable to change state to DETECT. Reason: Scan command in progress");
  }
}

void setup() {
  Serial.begin(9600);

  I2C_Wrapper::init(Slave, slaveID);
  I2C_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  I2C_Wrapper::registerDetectCommandHandler(&detectCommandHandler);

  motor.setup();
  scanner.setup();
}

void loop() {
  //Serial.println("Starting LaserMotorArm");
  motor.changeDirection();
  faceWall(3360/360, motor.singleRotation, 210, false);
  faceWall(1, 420, 0, true);
  //Serial.println("*Laser now faces wall");

  //Serial.println("Starting main loop...");
  while (1) {
    if (STATE == Detecting) {
      LaserScanner::totalRotations = 0;
      detectObjects(detectBegin, detectEnd, detectDistance);
    } else if (STATE == Scanning && waitingToScan) {
      waitingToScan = false;
      scanArea(3360 / 300, motor.singleRotation, Average);
      STATE = Waiting;
    }
  }
}
