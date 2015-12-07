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

void oneRotation(int fullSpin){
    startRotate();
    int scanTick = 0;
    int lastEncoderCount = -1;
    while(encoderCount < fullSpin){
      if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % 80 == 0) {
          scanTick += 1;
          Serial.print("[");
          Serial.print(scanTick);
          Serial.print(" | ");
          Serial.print(myLidarLite.distanceContinuous());
          Serial.println("]");
          lastEncoderCount = encoderCount;
      }
    }
    scanTick = 0;
    stopRotate();
    delay(2000);
    //correction = encoderCount - 3360;
}

int numTicks(){
    int correction = encoderCount - 3360;
    return correction;
}

int correctTicks(int correction){
    encoderCount = 0;
    startRotate();
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
    
    int i = 0;
    int fullSpin = 3360;
    int correction = 0;
    
    while(i < 2){
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        oneRotation(fullSpin);
        correction = numTicks();
        changeDirection();
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        
        correction = correctTicks(correction);
        //Serial.print("EncoderCount: ");
        //Serial.println(encoderCount);
        //correction = numTicks();
        fullSpin = correction + 3360;
        changeDirection();
        
        encoderCount = 0;
        i++;
        Serial.println(i);
    }   
}
