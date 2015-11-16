struct MoveCommand {
  int id, angle, magnitude;
};
struct MoveResponse {
  int id, angle, magnitude, reason;
};
struct RotateCommand {
  int id, angle;
};
struct RotateResponse {
  int id, angle, reason;
};
struct ScanResponse {
  int id, angle, distance; //Note angle a value between 0 & 42, as per laser gearing limitations
  bool last;
};

const int SPEED = 1; //in mm per millisecond

 //Use to access interrupt pins - may be needed to implement asynchronous stop
const byte INTERR_1 = 2;
const byte INTERR_2 = 3;

const int MAP_BOUNDS[] = {(0, 0, 41, 0), (41, 0, 41, 41), (41, 41, 0, 41), (0, 41, 0, 0)};

unsigned long timer;
int incomingByte, id, angle, magnitude;
String instruct;
bool brakes;

MoveCommand moveCom = {0, 0, 0};
MoveResponse moveResp = {0, 0, 0, 1};

RotateCommand rotCom = {0, 0};
RotateResponse rotResp = {0, 0, 1};

ScanResponse scanResp = {0, 0, 0, false};

void setup() {
  Serial.begin(9600);
  instruct = "";
  incomingByte = 0;
  id = 0;
  angle = 0;
  magnitude = 0;
  brakes = false;
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
    moveResp = moveRobot(moveCom);
    //Simulate data return to main program
    Serial.print("RESULT:\t");
    Serial.print(moveResp.id);
    Serial.print(",\t");
    Serial.print(moveResp.angle);
    Serial.print(",\t");
    Serial.print(moveResp.magnitude);
    Serial.print(",\t");
    Serial.println(moveResp.reason);
    //Reset global variables
    id = 0;
    angle = 0;
    magnitude = 0;
  }
  
  if(instruct == "rotate(") {
    makeRotateCommand();
    rotResp = rotate(rotCom);
    //Simulate data return to main program
    Serial.print("RESULT:\t");
    Serial.print(rotResp.id);
    Serial.print(",\t");
    Serial.print(rotResp.angle);
    Serial.print(",\t");
    Serial.println(rotResp.reason);
    //Reset global variables
    id = 0;
    angle = 0;
  }

  if(instruct == "scan(") {
    makeScanCommand();
    do{
      scanResp = scan(id);
      //Simulate data return to main program
      Serial.print("RESULT:\t");
      Serial.print(scanResp.id);
      Serial.print(",\t");
      Serial.print(scanResp.angle);
      Serial.print(",\t");
      Serial.print(scanResp.distance);
      Serial.print(",\t");
      Serial.println(scanResp.last);
    }
    while(!scanResp.last);
    //Reset global id variable
    id = 0;
  }

  if(instruct == "stopRobot();") {
    stopRobot(1); //Currently non-functional
    //Reset global variables
    brakes = false;
    instruct = "";
  }
}

struct MoveResponse moveRobot(struct MoveCommand com) {
  timer = millis();
  unsigned long time = ((com.magnitude / 10) / SPEED); //Estimate time actual robot would take. Magnitude brought down to millimetres, Time = Distance / Speed
  while(millis() - timer <= time) {
    //Wait for time to elapse, (attempt to) listen for stop command
    if(brakes) {
      unsigned long distTravelled = (SPEED * (millis() - timer)) * 10;
      return (MoveResponse){id, com.angle, (int)distTravelled, 1};
    }
  }
  return (MoveResponse){com.id, com.angle, com.magnitude, 0};
}

void stopRobot(int newID) { //Cannot get this to work for the life of me. Arduinos can only run one method at a time - how to overcome this??
  id = newID;
  brakes = true;
}

struct RotateResponse rotate(struct RotateCommand com) {
  timer = millis();
  while(millis() - timer <= com.angle) { //Yes, this means that the robot can turn 1 degree per ms. This is temporary.
    if(digitalRead(brakes)) {
      return (RotateResponse){id, (int)(millis() - timer), 1};
    }
  }
  return (RotateResponse){id, angle, 0};
}

struct ScanResponse scan(int id) {
  ScanResponse reply = {id, 0, 0, false};
  reply.angle = getLaserAngle(scanResp.angle);
  reply.distance = getLaserDist();
  reply.last = (reply.angle > 42); //Condition may need fine-tuning
  return reply;
}

void makeMoveCommand() {
    while(Serial.peek() != 44) { //Next char is not " , "
      //As Serial.read() returns single chars as ASCII values, an id of 127 would be read as 49 then 50 then 55.
      //To correct, we shift current id one place to right, read next number, subtract 48 to extract actual value from ASCII representation, then add to id
      id = id * 10 + (Serial.read() - 48);
    }
    Serial.print("ID:\t");
    Serial.println(id);
    Serial.read();
    Serial.read();
    while(Serial.peek() != 44) {
      angle = angle * 10 + (Serial.read() - 48);
    }
    Serial.print("ANGLE:\t");
    Serial.println(angle);
    Serial.read();
    Serial.read();
    while(Serial.peek() != 41) { //Next char is not " ) "
      magnitude = magnitude * 10 + (Serial.read() - 48);
    }
    Serial.print("MAGNITUDE:\t");
    Serial.println(magnitude);
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
    instruct = "";
    rotCom = {id, angle};
}
 //The following are placeholders while we await information on format of LIDAR data output
int getLaserAngle(int currAngle) {
  return currAngle + 1;
}

int getLaserDist() {
  return 20;
}
