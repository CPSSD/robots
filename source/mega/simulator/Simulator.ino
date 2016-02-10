#include "math.h"

struct MoveCommand {
  int id;
  float angle, magnitude;
};
struct MoveResponse {
  int id;
  float angle, magnitude, reason;
};
struct RotateCommand {
  int id, angle;
};
struct RotateResponse {
  int id, angle, reason;
};
struct ScanResponse {
  int id;
  float angle, distance; //Note angle a value between 0 & 42, as per laser gearing limitations
  bool last;
};
struct point {
  float x, y;
};
struct MapLine {
  point x1y1, x2y2;
};
struct EquationOfLine {
  point xy;
  float m, c;
  boolean isVertical;
};

const float SPEED = 0.1; //in mm per millisecond
const int STARTING_X = 20;
const int STARTING_Y = 20;
 //Use to access interrupt pins - may be needed to implement asynchronous stop
const byte INTERR_1 = 2;
const byte INTERR_2 = 3;

const MapLine MAP_BOUNDS[4] = {0, 0, 41, 0, 41, 0, 41, 41, 41, 41, 0, 41, 0, 41, 0, 0};
unsigned long moveTimer, rotateTimer, distTravelled;
int incomingByte, id, magnitude, angleInDegrees;
float angle;
point currentPosition, destination, nearestWall;
String instruct;
bool brakes, isCompletable, amMoving, amRotating;
MapLine ray;
EquationOfLine PERIMETER[4];
EquationOfLine equOfRay;

MoveCommand moveCom = {0, 0, 0};
MoveResponse moveResp = {0, 0, 0, 0};

RotateCommand rotCom = {0, 0};
RotateResponse rotResp = {0, 0, 0};

ScanResponse scanResp = {0, 0, 0, false};

void setup() {
  Serial.begin(9600);
  instruct = "";
  incomingByte = 0;
  id = 0;
  angle = 0.0;
  angleInDegrees = 0;
  magnitude = 0;
  currentPosition.x = STARTING_X;
  currentPosition.y = STARTING_Y;
  brakes = false;
  isCompletable = false;
  amMoving = false;
  amRotating = false;
  for(int i = 0; i < 4; i++) {
    PERIMETER[i] = getEquationOfLine(MAP_BOUNDS[i]);
  }
}

void loop() {
  
  if(Serial.available() > 0) {
    //Re-construct the instruction issued
    incomingByte = Serial.read();
    instruct += (char)incomingByte;
    delay(10); //Required to allow read buffer to recover
  }
    
  if(instruct == "moveRobot(") {
    makeMoveCommand();
    moveRobot(moveCom);
    //Reset global variables
    id = 0;
    angle = 0;
    magnitude = 0;
  }
  if(instruct == "rotate(") {
    makeRotateCommand();
    rotate(rotCom);
    id = 0;
    angle = 0;
  }
  if(instruct == "scan(") {
    makeScanCommand();
    do{
      scanResp = scan(id, (angleInDegrees * PI) / 180);
      serialReply(scanResp);
      angleInDegrees = angleInDegrees + 8; //This will only allow for scans between 0 and 357 degrees
    }
    while(!scanResp.last);
    //Reset global id variable
    id = 0;
    angleInDegrees = 0;
    instruct = "";
  }
  if(instruct == "stopRobot();") {
    stopRobot(); //Currently non-functional
  }
  if(brakes) {
    wrapUp(1);
    brakes = false;
  }
  if(amMoving && millis() - moveTimer >= 100UL) {
    moveResp = giveHeartbeatMove(moveCom, (millis() - moveTimer));
    //serialReply(moveResp);
    if(moveResp.reason > 0) {
      serialReply(moveResp);
      currentPosition = destination;
    }
  }
  if(amRotating && millis() - rotateTimer >= 1000UL) {
    rotResp = giveHeartbeatRotate(rotCom);
  }
}

void moveRobot(struct MoveCommand com) {
  ray = makeLineFromPolar(com.angle, com.magnitude);
  equOfRay = getEquationOfLine(ray);
  destination = checkIfCompletable(ray, equOfRay, PERIMETER);
  amMoving = true;
  moveTimer = millis();
}

struct MoveResponse giveHeartbeatMove(struct MoveCommand com, unsigned long time) {
  distTravelled = distTravelled + ((SPEED * time) * 10);
  if(distTravelled >= com.magnitude) {
    amMoving = false;
    moveTimer = millis();
    return (MoveResponse){com.id, com.angle, com.magnitude, 1};
  }
  moveTimer = millis();
  return (MoveResponse){id, com.angle, (int)distTravelled, 0};
}

struct point checkIfCompletable(struct MapLine moveLine, struct EquationOfLine robotLine, struct EquationOfLine obstacles[]) {
  //For each line, check for valid interception point, get interception point
  point validInterceptPoints[4];
  int counterVIP = 0;
  for(int i = 0; i < 4; i++) {
    if(hasInterception(obstacles[i], robotLine)) {
      Serial.println("Has intercept point");
      validInterceptPoints[counterVIP] = getInterceptPoint(robotLine, obstacles[i]);
      counterVIP++;
    }
  }
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = moveLine.x2y2.x - moveLine.x1y1.x;
  float diffInYValuesDest = moveLine.x2y2.y - moveLine.x1y1.y;
  //Do the same for the first point
  float diffInXValuesWallOne = validInterceptPoints[0].x - robotLine.xy.x;
  float diffInYValuesWallOne = validInterceptPoints[0].y - robotLine.xy.y;
  //If the signs match, that's the point in consideration, else it's the other one
  if(( (diffInXValuesDest > 0.0 && diffInXValuesWallOne > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWallOne == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWallOne < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWallOne > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWallOne == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWallOne < 0.0) )) {
    nearestWall = validInterceptPoints[0];
  }
  else {
    nearestWall = validInterceptPoints[1];
  }
  //Get distance between robot and destination, and robot and wall
  float distBetweenRobotAndDest = getDistBetweenTwoPoints(moveLine.x1y1, moveLine.x2y2);
  float distBetweenRobotAndNearWall = getDistBetweenTwoPoints(robotLine.xy, nearestWall);
  //If distance between robot and destination is shorter than between robot and wall, return destination, else return interception point
  if(distBetweenRobotAndDest < distBetweenRobotAndNearWall) {
    isCompletable = true;
    return moveLine.x2y2;
  }
  moveCom.magnitude = (int)distBetweenRobotAndNearWall;
  return nearestWall;
}

void stopRobot() {
  brakes = true;
}

void wrapUp(int newID) {
  if(amMoving) {
    moveResp = giveHeartbeatMove(moveCom, (millis() - moveTimer));
    amMoving = false;
    moveTimer = millis();
    serialReply(moveResp);
    currentPosition = {(currentPosition.x + moveCom.magnitude), (currentPosition.y + moveCom.magnitude)};
  }
  if(amRotating) {
    rotResp = giveHeartbeatRotate(rotCom);
    amRotating = false;
    rotateTimer = millis();
    serialReply(rotResp);
  }
  id = newID;
  angle = 0;
  magnitude = 0;
  instruct = "";
}

struct RotateResponse giveHeartbeatRotate(struct RotateCommand com) {
  angle = angle + 1;
  if(angle >= com.angle) {
    rotateTimer = millis();
    return (RotateResponse){id, com.angle, 1};
  }
  rotateTimer = millis();
  return (RotateResponse){id, angle, 0};
}

void rotate(struct RotateCommand com) {
  rotateTimer = millis();
}

struct ScanResponse scan(int id, float angle) {
  ScanResponse reply = {id, angle, 66.0, false};
  ray = makeLineFromPolar(reply.angle, reply.distance);
  equOfRay = getEquationOfLine(ray);
  nearestWall = checkIfCompletable(ray, equOfRay, PERIMETER);
  reply.distance = getDistBetweenTwoPoints(ray.x1y1, nearestWall);
  reply.last = (angleInDegrees == 357);
  return reply;
}

void makeMoveCommand() {
    while(Serial.peek() != 44) { //Next char is not " , "
      //As Serial.read() returns single chars as ASCII values, an id of 127 would be read as 49 then 50 then 55.
      //To correct, we shift current id one place to right, read next number, subtract 48 to extract actual value from ASCII representation, then add to id
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 44) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    angle = (angle * PI) / 180.0;
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      magnitude = magnitude * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
    moveCom = {id, angle, magnitude};
}

void makeRotateCommand() {
    while(Serial.peek() != 44) {
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
}

void makeScanCommand() {
    while(Serial.peek() != 41) {
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
    rotCom = {id, angle};
}

void serialReply(struct MoveResponse moveResp) {
    Serial.print("RESULT:\t");
    Serial.print(moveResp.id);
    Serial.print(",\t");
    Serial.print(moveResp.angle);
    Serial.print(",\t");
    Serial.print(moveResp.magnitude);
    Serial.print(",\t");
    Serial.println(moveResp.reason);
}

void serialReply(struct RotateResponse rotResp) {
    Serial.print("RESULT:\t");
    Serial.print(rotResp.id);
    Serial.print(",\t");
    Serial.print(rotResp.angle);
    Serial.print(",\t");
    Serial.println(rotResp.reason);
}

void serialReply(struct ScanResponse scanResp) {
    Serial.print("RESULT:\t");
    Serial.print(scanResp.id);
    Serial.print(",\t");
    Serial.print(scanResp.angle);
    Serial.print(",\t");
    Serial.print(scanResp.distance);
    Serial.print(",\t");
    Serial.println(scanResp.last);
}

float getDistBetweenTwoPoints(point p1, point p2) {
  return sqrt((pow(p2.x - p1.x, 2)) + (pow(p2.y - p1.y, 2)));
}

struct MapLine makeLineFromPolar(float angle, float distance) {
  MapLine temp;
  temp.x1y1 = {currentPosition.x, currentPosition.y};
  temp.x2y2 = {(currentPosition.x + (distance * cos(angle))), (currentPosition.y + (distance * sin(angle)))};
  return temp;
}

struct EquationOfLine getEquationOfLine(MapLine line) {
  EquationOfLine equ;
  equ.xy.x = line.x1y1.x;
  equ.xy.y = line.x1y1.y;
  if((line.x2y2.x - equ.xy.x) == 0.00) {
    equ.isVertical = true;
  }
  else {
    equ.isVertical = false;
  }
  equ.m = (line.x2y2.y - equ.xy.y) / (line.x2y2.x - equ.xy.x); //Warning: this will be NaN if line is vertical (as denominator will be zero). The boolean allows for avoiding this posing a problem
  equ.c = -(equ.m * equ.xy.x) + equ.xy.y;
  return equ;
}

boolean hasInterception(EquationOfLine border, EquationOfLine robotMoveLine) {
  if((robotMoveLine.isVertical && border.isVertical) || (robotMoveLine.m == border.m && (!robotMoveLine.isVertical || !border.isVertical))) { //Lines are parallel
    return false;
  }
  else if(robotMoveLine.isVertical || border.isVertical) {
    if(robotMoveLine.isVertical) {
      return true;
    }
    if(border.isVertical) {
      //Line is in point-slope form (y = line.m(x - line.x) + line.y
      //Convert to slope-intercept form: y = (line.m * x) + (line.m * line.x) + line.y
      //y = line.m * x + ((line.m * line.x) + line.y)
      float intercept = ((robotMoveLine.m * border.xy.x) + ((robotMoveLine.m * robotMoveLine.xy.x) + robotMoveLine.xy.y));
      if(intercept >= 0.0 && intercept <= 41.0) {
        return true;
      }
      else {
        return false;
      }
    }
  }
  else {
    return true;
  }
}

struct point getInterceptPoint(EquationOfLine robotLine, EquationOfLine other) {
  point intercept;
  //Special case: one of the lines is vertical.
  //In this event, the x-value of the intercept point will be constant for all values of y.
  //Therefore, we can substitute that value into the equation of the non-vertical line and solve for y.
  if(other.isVertical) {
    intercept.x = other.xy.x;
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  else if(robotLine.isVertical) {
    intercept.x = robotLine.xy.x;
    intercept.y = (other.m * intercept.x) + other.c;
  }
  else {
    //We set the two equations equal to each other and manipulate the expression to solve for x.
    // m1x + c1 = m2x + c2 -> m1x - m2x = c2 - c1 -> x = (c2 - c1) / (m1 - m2)
    float denominator = robotLine.m - other.m;
    float numerator = other.c - robotLine.c;
    intercept.x = numerator / denominator;
    //Substitute the x-value into either equation of the lines and solve for y
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  return intercept;
}
