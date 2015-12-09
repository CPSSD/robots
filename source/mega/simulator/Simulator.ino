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
EquationOfLine PERIMETER[4];
MapLine ray;
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
  /*Serial.println(PERIMETER[0].m);
  Serial.println(PERIMETER[1].m);
  Serial.println(PERIMETER[2].m);
  Serial.println(PERIMETER[3].m);
  Serial.println(PERIMETER[4].m);
  Serial.println(MAP_BOUNDS[0].x1);
  Serial.println(MAP_BOUNDS[0].y1);
  Serial.println(MAP_BOUNDS[0].x2);
  Serial.println(MAP_BOUNDS[0].y2);
  Serial.println(MAP_BOUNDS[1].x1);
  Serial.println(MAP_BOUNDS[1].y1);
  Serial.println(MAP_BOUNDS[1].x2);
  Serial.println(MAP_BOUNDS[1].y2);
  Serial.println(MAP_BOUNDS[2].x1);
  Serial.println(MAP_BOUNDS[2].y1);
  Serial.println(MAP_BOUNDS[2].x2);
  Serial.println(MAP_BOUNDS[2].y2);
  Serial.println(MAP_BOUNDS[3].x1);
  Serial.println(MAP_BOUNDS[3].y1);
  Serial.println(MAP_BOUNDS[3].x2);
  Serial.println(MAP_BOUNDS[3].y2);*/
}

void loop() {
  //Since the various protocols, libraries and APIs had not been fleshed out yet, issuing commands over Serial was the only way to simulate function calls from the main program.
  //However, this required a lot of parsing, which was messy.




  
  if(Serial.available() > 0) {
    //Re-construct the instruction issued
    incomingByte = Serial.read();
    instruct += (char)incomingByte;
    delay(10); //Required to allow read buffer to recover
    
    //Serial.print("I received: ");
    //Serial.println(incomingByte, DEC);
  }
    
  if(instruct == "moveRobot(") {
    makeMoveCommand();
    /*Serial.print("Magnitude at creation of moveCom:\t");
    Serial.println(moveCom.magnitude);*/
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
      //Simulate data return to main program
      Serial.print("RESULT:\t");
      Serial.print(scanResp.id);
      Serial.print(",\t");
      Serial.print(scanResp.angle);
      Serial.print(",\t");
      Serial.print(scanResp.distance);
      Serial.print(",\t");
      Serial.println(scanResp.last);
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
    /*Serial.print("Magnitude going into heartbeat method:\t");
    Serial.println(moveCom.magnitude);*/
    moveResp = giveHeartbeatMove(moveCom, (millis() - moveTimer));
    serialReply(moveResp);
    if(moveResp.reason > 0) {
      Serial.print("Millis:\t");
      Serial.print(millis());
      Serial.print(",\t");
      Serial.print("Timer:\t");
      Serial.print(moveTimer);
      Serial.print(",\t");
      Serial.print("Millis minus timer:\t");
      Serial.println(millis() - moveTimer);
      //serialReply(moveResp);
      currentPosition = destination;
    }
  }
  if(amRotating && millis() - rotateTimer >= 1000UL) {
    rotResp = giveHeartbeatRotate(rotCom);
  }
}

void moveRobot(struct MoveCommand com) {
  ray = makeLineFromPolar(com.angle, com.magnitude);
  /*Serial.print("Magnitude after makeLineFromPolar:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);*/
  equOfRay = getEquationOfLine(ray);
  /*Serial.print("Magnitude after getEquationOfLine:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);*/
  destination = checkIfCompletable(ray, equOfRay, PERIMETER);
  /*Serial.print("Magnitude after checkIfCompletable:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);*/
  amMoving = true;
  /*Serial.print("Magnitude after all of moveRobot:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);*/
  moveTimer = millis();
}

struct MoveResponse giveHeartbeatMove(struct MoveCommand com, unsigned long time) {
  /*Serial.print("Magnitude at Top of giveHeartbeatMove:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);
  Serial.print("Distance travelled before recalculating:\t");
  Serial.println(distTravelled);*/
  distTravelled = distTravelled + ((SPEED * time) * 10);
  /*Serial.print("Magnitude after recalculating distTravelled:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);
  Serial.print("Distance travelled after recalculating:\t");
  Serial.println(distTravelled);*/
  if(distTravelled >= com.magnitude) {
    amMoving = false;
    /*if(isCompletable) {
      moveTimer = millis();
      return (MoveResponse){com.id, com.angle, com.magnitude, 1};
    }*/
  /*Serial.print("Magnitude when distTravelled >= magnitude:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);*/
    moveTimer = millis();
    return (MoveResponse){com.id, com.angle, com.magnitude, 1};
  }
  /*if(isCompletable) {
    moveTimer = millis();
    return (MoveResponse){id, com.angle, (int)distTravelled, 1};
  }*/
  /*Serial.print("Magnitude when distTravelled < magnitude:\t");
  Serial.print(com.magnitude);
  Serial.print(" vs. ");
  Serial.println(moveCom.magnitude);
  Serial.print("Distance travelled at end of giveHeartbeatMove:\t");
  Serial.println(distTravelled);*/
  moveTimer = millis();
  return (MoveResponse){id, com.angle, (int)distTravelled, 0};
}

struct point checkIfCompletable(struct MapLine moveLine, struct EquationOfLine robotLine, struct EquationOfLine obstacles[]) {
  //For each line, check for valid interception point, get interception point
  point validInterceptPoints[4];
  int counterVIP = 0;
  for(int i = 0; i < 4; i++) {
    Serial.print("Wall ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print("X:\t");
    Serial.print(obstacles[i].xy.x);
    Serial.print("\tY:\t");
    Serial.println(obstacles[i].xy.y);
    if(hasInterception(obstacles[i], robotLine)) {
      Serial.println("Has intercept point");
      validInterceptPoints[counterVIP] = getInterceptPoint(robotLine, obstacles[i]);
      /*Serial.print("X:\t");
      Serial.print(validInterceptPoints[counterVIP].x);
      Serial.print("\tY:\t");
      Serial.println(validInterceptPoints[counterVIP].y);*/
      counterVIP++;
    }
  }
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = moveLine.x2y2.x - moveLine.x1y1.x;
  float diffInYValuesDest = moveLine.x2y2.y - moveLine.x1y1.y;
  //Do the same for the first point
  float diffInXValuesWallOne = validInterceptPoints[0].x - robotLine.xy.x;
  Serial.println(validInterceptPoints[0].x);
  Serial.println(robotLine.xy.x);
  Serial.println(diffInXValuesWallOne);
  float diffInYValuesWallOne = validInterceptPoints[0].y - robotLine.xy.y;
  Serial.println(diffInYValuesWallOne);
  //If the signs match, that's the point in consideration, else it's the other one
  if(( (diffInXValuesDest > 0.0 && diffInXValuesWallOne > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWallOne == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWallOne < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWallOne > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWallOne == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWallOne < 0.0) )) {
    nearestWall = validInterceptPoints[0];
    Serial.println("First Wall");
  }
  else {
    nearestWall = validInterceptPoints[1];
    Serial.println("Second Wall");
  }
  //Get distance between robot and destination, and robot and wall
  
  float distBetweenRobotAndDest = getDistBetweenTwoPoints(moveLine.x1y1, moveLine.x2y2);
  Serial.println(distBetweenRobotAndDest);
  float distBetweenRobotAndNearWall = getDistBetweenTwoPoints(robotLine.xy, nearestWall);
  Serial.println(distBetweenRobotAndNearWall);
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

void wrapUp(int newID) { //Cannot get this to work for the life of me. Arduinos can only run one method at a time - how to overcome this??
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
    rotateTimer = 101112UL;
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
  reply.last = (angleInDegrees == 357); //Condition may need fine-tuning
  return reply;
}

void makeMoveCommand() {
    while(Serial.peek() != 44) { //Next char is not " , "
      //As Serial.read() returns single chars as ASCII values, an id of 127 would be read as 49 then 50 then 55.
      //To correct, we shift current id one place to right, read next number, subtract 48 to extract actual value from ASCII representation, then add to id
      id = id * 10 + (Serial.read() - 48);
    }
    //Serial.print("ID:\t");
    //Serial.println(id);
    Serial.read();
    Serial.read();
    while(Serial.peek() != 44) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    angle = (angle * PI) / 180.0;
    //Serial.print("ANGLE:\t");
    //Serial.println(angle);
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      magnitude = magnitude * 10 + (Serial.read() - 48);
    }
    //Serial.print("MAGNITUDE:\t");
    //Serial.println(magnitude);
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
    //Serial.print("ID:\t");
    //Serial.println(id);
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    //Serial.print("ANGLE:\t");
    //Serial.println(angle);
    Serial.read();
    Serial.read();
    Serial.read();
    instruct = "";
}

void makeScanCommand() {
    while(Serial.peek() != 41) {
      id = id * 10 + (Serial.read() - 48);
    }
    //Serial.print("ID:\t");
    //Serial.println(id);
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

 //The following are placeholders while we await information on format of LIDAR data output
int getLaserAngle(int currAngle) {
  return currAngle + 1;
}

float getDistBetweenTwoPoints(point p1, point p2) {
  return sqrt((pow(p2.x - p1.x, 2)) + (pow(p2.y - p1.y, 2)));
}

struct MapLine makeLineFromPolar(float angle, float distance) {
  //Serial.print("Distance inside makeLineFromPolar:\t");
  //Serial.println(distance);
  MapLine temp;
  temp.x1y1 = {currentPosition.x, currentPosition.y};
  /*if(angle == 0 || angle == 360) {
    temp.x2y2
    
  }*/
  /*Serial.print("Distance to travel on x-axis:\t");
  Serial.println(distance * cos(angle));
  Serial.print("Distance to travel on y-axis:\t");
  Serial.println(distance * sin(angle));*/
  temp.x2y2 = {(currentPosition.x + (distance * cos(angle))), (currentPosition.y + (distance * sin(angle)))};
  Serial.print("Origin:\t");
  Serial.print(temp.x1y1.x);
  Serial.print(",\t");
  Serial.print(temp.x1y1.y);
  Serial.println();
  Serial.print("Destination:\t");
  Serial.print(temp.x2y2.x);
  Serial.print(",\t");
  Serial.print(temp.x2y2.y);
  Serial.println();
  return temp;
}

struct EquationOfLine getEquationOfLine(MapLine line) {
  EquationOfLine equ;
  equ.xy.x = line.x1y1.x;
  equ.xy.y = line.x1y1.y;
  //Serial.println((line.x2y2.x - equ.xy.x));
  if((line.x2y2.x - equ.xy.x) == 0.00) {
    //Serial.println("Is vertical");
    equ.isVertical = true;
  }
  else {
    //Serial.println("Is NOT vertical");
    equ.isVertical = false;
    //Serial.print("Slope of the line:\t");
    //Serial.println(equ.m);
  }
  equ.m = (line.x2y2.y - equ.xy.y) / (line.x2y2.x - equ.xy.x);
  equ.c = -(equ.m * equ.xy.x) + equ.xy.y;
  return equ;
}

boolean hasInterception(EquationOfLine border, EquationOfLine robotMoveLine) {
  /*Serial.print("Is robot's path vertical?\t");
  Serial.println(robotMoveLine.isVertical);
  Serial.print("Is the wall vertical?\t");
  Serial.println(border.isVertical);
  Serial.println((robotMoveLine.isVertical && border.isVertical));
  Serial.println((robotMoveLine.m == border.m));
  Serial.println(!(robotMoveLine.isVertical && border.isVertical));*/
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
  //Normally, to find the interception point of two lines, you would set the equations equal to each other (since their y values would be equal at the interception point), solve for x, then use that value to get the y value.
  //However, the Arduino does not allow you to have uninitialised variables on the right-hand side of an assignment (it's not exactly SWI Prolog, now).
  //The only way I could think of doing this was to substitute 1 for x (as that shouldn't affect the equation).
  if(other.isVertical) {
    intercept.x = other.xy.x;
    //Serial.println(robotLine.c);
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  else if(robotLine.isVertical) {
    Serial.println("Robot is vertical");
    intercept.x = robotLine.xy.x;
    intercept.y = (other.m * intercept.x) + other.c;
  }
  else {
    float denominator = robotLine.m - other.m; //x terms on left
    float numerator = other.c - robotLine.c; //constants on right
    intercept.x = numerator / denominator;
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  return intercept;
}

/*struct MoveResponse moveRobot_Obsolete(struct MoveCommand com) {
  unsigned long time = ((com.magnitude / 10) / SPEED); //Estimate time actual robot would take. Magnitude brought down to millimetres, Time = Distance / Speed
  while(millis() - moveTimer <= time) {
    //Wait for time to elapse, (attempt to) listen for stop command
    if(brakes) {
      distTravelled = (SPEED * (millis() - moveTimer)) * 10;
      return (MoveResponse){id, com.angle, (int)distTravelled, 1};
    }
  }
  return (MoveResponse){com.id, com.angle, com.magnitude, 0};
}*/
