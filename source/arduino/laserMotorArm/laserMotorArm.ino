#include <LIDARLite.h>

#include <Motor.h>
#include <LaserScanner.h>
#include <Wire.h>

Motor motor = Motor();
LaserScanner scanner = LaserScanner();

void faceWall(){
  //while(1)
  motor.registerRotationFunction(&LaserScanner::getContinuousReading);
  LaserReading goal = LaserReading{-1, -1};
  int offset = motor.rotateWithCorrection(motor.singleRotation);
  motor.encoderCount = 0;

  for (int i = 0; i < 42; i++){
    if (goal.distance == -1 || (LaserScanner::lastRotationData[i].distance < goal.distance && LaserScanner::lastRotationData[i].distance  >= 5)){
      goal = LaserScanner::lastRotationData[i];
    }
  }

  Serial.print(goal.angle);
  Serial.print(", ");
  Serial.print(goal.distance);
  Serial.print(" ||| ");
  Serial.println(motor.singleRotation);

  int ticksToTravel = (motor.singleRotation * goal.angle) / 42;

  Serial.print("Wall @ ");
  Serial.print(goal.angle);
  Serial.print(" | In ticks: ");
  Serial.println(ticksToTravel);

  if (ticksToTravel < 0){
    while (ticksToTravel < 0){
      ticksToTravel += motor.singleRotation;
    }

    Serial.print("New Ticks to travel: ");
    Serial.println(ticksToTravel);
  }

  if (goal.angle <= -1) {
    Serial.println("*Error, angle was set to negative value. Halted movement to avoid infinite loop");
    goal.angle = 0;
  }
  
  motor.encoderCount = 0;
  int correction = motor.rotateWithCorrection(ticksToTravel + offset);
  
  Serial.print("off by about ");
  Serial.println(correction);
  
  motor.changeDirection();
}

void setup(){
    motor.setup();
    scanner.setup();
    Serial.begin(9600);
}

void loop(){
    faceWall();
    Serial.println("*Laser now faces wall");
    
    int fullSpin = motor.singleRotation;
    
    //motor.rotateWithCorrection(fullSpin);
    //motor.rotateByAngle(360);
  
    while(1);
}
