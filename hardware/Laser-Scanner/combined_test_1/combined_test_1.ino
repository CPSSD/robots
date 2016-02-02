//
// Test of motor, encoder and continuous laser scanning
//

#include <Wire.h>
#include <LIDARLite.h>

// Laser ISR
const int laserInterrupt = 1; // Pin 3
volatile unsigned int newReading = false;

void laserISR() {
  newReading = true;
}

LIDARLite myLidarLite;

// Motor driver pins：
const int E1 = 5; 
const int M1 = 4;
const int E2 = 6;                     
const int M2 = 7;                       
 
// Encoder
const int encoderInterrupt = 0; // Pin 2
volatile unsigned int encoderCount = 0;
volatile unsigned int completedOneRevolution = false;

void encoderISR() {
  encoderCount = (encoderCount + 1) % 3360;
  if (encoderCount == 0) {
     completedOneRevolution = true;
  }
}

void setup()
{
    attachInterrupt(encoderInterrupt,encoderISR,FALLING);
    attachInterrupt(laserInterrupt,laserISR,FALLING);

    myLidarLite.begin();
    myLidarLite.beginContinuous();

    digitalWrite(M1,HIGH);  
    digitalWrite(M2, HIGH);      
    analogWrite(E1, 255);  

    Serial.begin(115200);
}

void loop()
{
  if(false) { //newReading){
    newReading = false;
    int distance = myLidarLite.distanceContinuous();
    //Serial.print(encoderCount);
    //Serial.print(" : ");
    //Serial.println(distance);
    delay(10);
  }
  if (completedOneRevolution) {
    completedOneRevolution = false;
    Serial.print(encoderCount);
    Serial.println(" : stopping ...");
    analogWrite(E1, 0);  
    myLidarLite.beginContinuous(false);
  }
}


