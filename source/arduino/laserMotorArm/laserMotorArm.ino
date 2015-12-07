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
const int speed = 255;

boolean anticlockwise = true;
int correction = 0;

LIDARLite myLidarLite;

struct LaserReading {
  int angle;
  int distance;
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
  for(int i = 1; i < 41; i++){
    if (hasApex(lastRotationData[i-1], lastRotationData[i], lastRotationData[i+1])){
      Serial.print("Found Apex at: ");
      Serial.println(lastRotationData[i].angle);
      if (goal.distance == -1 || goal.distance > lastRotationData[i].distance){
        goal = lastRotationData[i];
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
  oneRotation(goal.angle*80 + offset);
  changeDirection();
  
  correctTicks(numTicks(goal.angle*80 + offset));
  changeDirection();
}

bool hasApex(LaserReading left, LaserReading middle, LaserReading right){
  if (left.distance == -1 || middle.distance == -1 || right.distance == -1){
    return false;
  }
  if (left.distance > middle.distance && right.distance > middle.distance){
    return true;
  }
  return false;
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
