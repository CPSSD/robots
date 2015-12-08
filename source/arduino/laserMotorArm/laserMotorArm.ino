#include <Wire.h>
#include <LIDARLite.h>

// Motor & encoder drivers

const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;

volatile unsigned long encoderCount = 0;
volatile unsigned int completedOneRevolution = false;
const int interupt = 0;
const int speed = 200;

boolean anticlockwise = true;
int correction = 0;

LIDARLite myLidarLite;

struct LaserReading {
  int angle;
  int distance;
};

struct Line {
  int x1;
  int y1;
  int x2;
  int y2;
  int slope;
};

LaserReading lastRotationData[42];

void encoderISR(){
    encoderCount++;
}

void rotateAnticlockwise(){
    anticlockwise = true;
    digitalWrite(M1,HIGH);
    digitalWrite(M2,HIGH);

}

void rotateClockwise(){
    anticlockwise = false;
    digitalWrite(M1,LOW);
    digitalWrite(M2,LOW);
}

void changeDirection(){
    if(anticlockwise){
        rotateClockwise();
    }else{
        rotateAnticlockwise();
    }
}

void startRotate(){
    analogWrite(E1,speed);
}

void stopRotate(){
    analogWrite(E1,0);
}

void faceWall(){
  oneRotation(3360);

  LaserReading goal = LaserReading{-1, -1};
  for(int i = 0; i < 42; i++){
    int j = i+1;
    int k = i+2;
    int l = i+3;
    
    if (j >= 42){
      j -= 42;
    }
    if (k >= 42){
      k -= 42;
    }
    if (l >= 42){
      l -= 42;
    }

    LaserReading apex = hasApex(lastRotationData[i], lastRotationData[j], lastRotationData[k], lastRotationData[l]);
    if (apex.angle != -1){ 
      Serial.print("Found Apex at: ");
      Serial.println(apex.angle);
      printData(Line{apex.angle, apex.distance, 0, 0, 0}, Line{0, 0, 0, 0, 0});
      
      if (goal.distance == -1 || goal.distance < apex.distance){
        Serial.print("Found new closer intersection @ ");
        Serial.println(apex.angle);
        goal = apex;
      }
    }
  }
  Serial.print("Wall @ ");
  Serial.print(goal.angle);
  Serial.print(" | In ticks: ");
  Serial.println(goal.angle*80);
  
  changeDirection();
  int offset = correctTicks(numTicks(3360));
  changeDirection();
  
  encoderCount = 0;
  oneRotation((goal.angle*80) + offset);
  changeDirection();
  
  Serial.print("off by about ");
  Serial.println(correctTicks(numTicks(goal.angle*80 + offset)));
  changeDirection();
}

// WILL REMOVE RIGHT AFTER I FINISH;
void printData(Line l1, Line l2){
    Serial.print("(");
    Serial.print(l1.x1);
    Serial.print(",");
    Serial.print(l1.y1);
    Serial.print("), ");
    Serial.print("(");
    Serial.print(l1.x2);
    Serial.print(",");
    Serial.print(l1.y2);
    Serial.print(") => ");
    Serial.print("C: ");
    Serial.print(getConstant(l1));
    Serial.print(", => ");
    Serial.println(l1.slope);
    
    Serial.print("(");
    Serial.print(l2.x1);
    Serial.print(",");
    Serial.print(l2.y1);
    Serial.print("), ");
    Serial.print("(");
    Serial.print(l2.x2);
    Serial.print(",");
    Serial.print(l2.y2);
    Serial.print(") => ");
    Serial.print("C: ");
    Serial.print(getConstant(l2));
    Serial.print(", => ");
    Serial.println(l2.slope);
    Serial.print("... Intersection @ ");
}

LaserReading hasApex(LaserReading p1, LaserReading p2, LaserReading p3, LaserReading p4){
  Line l1 = Line{p1.angle, p1.distance, p2.angle, p2.distance};
  Line l2 = Line{p3.angle, p3.distance, p4.angle, p4.distance};
  if (p1.distance == -1 || p2.distance == -1 || p3.distance == -1 || p4.distance == -1){
    return LaserReading{-1, -1};
  }

  if (l1.y1 >= l1.y2 && l2.y2 >= l2.y1){
    l1.slope = getSlope(l1);
    l2.slope = getSlope(l2);

    int intersection = getIntersection(l1, l2);
    Serial.print("* => ");
    Serial.println(intersection);
    if (int(intersection) >= int(p1.angle) && int(intersection) <= int(p4.angle)){
      Serial.println("*** Did it work ***");
      return LaserReading{intersection, getDistanceAtY(l1, intersection)};
    }
  }

  return LaserReading{-1, -1};
}

int getDistanceAtX(Line line, int y){
  // y = mx + c
  return (y - getConstant(line)) / getSlope(line);
}

int getDistanceAtY(Line line, int x){
  // y = mx + c
  return (getSlope(line) * x) + getConstant(line);
}

// returns value of x at intersection
int getIntersection(Line l1, Line l2){
  int intersection = (getConstant(l2) - getConstant(l1))/(l1.slope - l2.slope);
  Serial.print("Intersection at ");
  Serial.print(intersection);
  if (intersection < 0){
    intersection = -1 * intersection;
  }
  intersection = getDistanceAtX(l1, intersection);
  if (intersection < 0){
    intersection = -1 * intersection;
  }
  Serial.print(" | ");
  Serial.println(intersection);
  return intersection;
}

int getConstant(Line line){
  // y - y1 = m(x - x1);
  // y = mx 
  return (line.slope*line.x1) + line.y1;
}

int getSlope(Line line){
  if (line.x1 == line.x2) {
    return 0;
  }
  return (line.y1 - line.y2) / (line.x1 - line.x2);
}

void oneRotation(int fullSpin){
    startRotate();
    int scanTick = 0;
    int lastEncoderCount = -1;
    while(encoderCount < fullSpin){
      if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % 80 == 0) {
          scanTick += 1;
          if (scanTick >= 0 && scanTick <= 42){
            lastRotationData[scanTick-1] = LaserReading{scanTick-1, int(myLidarLite.distanceContinuous())};
            if (lastRotationData[scanTick-1].distance >= 1061) {
              lastRotationData[scanTick-1].distance = -1; 
            }
          }
          Serial.print("[");
          Serial.print(scanTick-1);
          Serial.print(" | ");
          Serial.print(lastRotationData[scanTick-1].distance);
          Serial.println("]");
          lastEncoderCount = encoderCount;
      }
    }
    scanTick = 0;
    stopRotate();
    delay(2000);
    //correction = encoderCount - 3360;
}

int numTicks(int offset){
    int correction = encoderCount - offset;
    return correction;
}

int correctTicks(int correction){
    encoderCount = 0;
    startRotate();
    Serial.print("***** ");
    Serial.println(correction);
    while(encoderCount < correction){}
    stopRotate();
    delay(2000);
    //Serial.println(encoderCount);
    return encoderCount - correction;
}

void setup(){
    attachInterrupt(interupt,encoderISR,FALLING);
    Serial.begin(9600);

    // Laser Scanner Setup.
    myLidarLite.begin();
    myLidarLite.beginContinuous();
    
    rotateAnticlockwise();
}

void loop(){
    faceWall();
    while(true){}
    
    int i = 0;
    int fullSpin = 3360;
    int correction = 0;
    
    while(i < 2){
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        oneRotation(fullSpin);
        correction = numTicks(3360);
        changeDirection();
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        
        correction = correctTicks(correction);
        //Serial.print("EncoderCount: ");
        //Serial.println(encoderCount);
        //correction = numTicks(3360);
        fullSpin = correction + 3360;
        changeDirection();
        
        encoderCount = 0;
        i++;
        Serial.println(i);
    }   
}
