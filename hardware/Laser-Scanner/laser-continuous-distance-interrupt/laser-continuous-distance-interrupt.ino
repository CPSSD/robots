

#include <Wire.h>
#include <LIDARLite.h>

// Laser ISR
const int interrupt = 1; // Pin 3
volatile unsigned int newReading = false;

void laserISR() {
  newReading = true;
}


LIDARLite myLidarLite;

void setup() {
  attachInterrupt(interrupt,laserISR,FALLING);
  Serial.begin(115200);
  myLidarLite.begin();
  myLidarLite.beginContinuous();
  pinMode(3, INPUT);
}

void loop() {
  if(newReading){
    newReading = false;
    Serial.println(myLidarLite.distanceContinuous());
  }
}
