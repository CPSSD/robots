#include <PID_v1.h>
#include <Wire.h>
#include <I2C_Wrapper.h>
#include <VectorMath.h>
#include <Shared_Structs.h>
#include <WheelMotors.h>

const byte slaveAddress = 17;

WheelMotors motor = WheelMotors();
VectorMath motorSpeed = VectorMath();

unsigned long startingTime,timePassed;

boolean first = true;

void setup(){
    I2C_Wrapper::init(Slave, slaveAddress);
    I2C_Wrapper::registerMoveCommandHandler(&motor.moveCommandHandler);
    I2C_Wrapper::registerStopCommandHandler(&motor.stopCommandHandler);
    //I2C_Wrapper::sendStopCommand(15);
    
    motor.setup();
    
    motor.commandHandled = true;
    motor.finished = false;
    //first = true;

    Serial.begin(9600);  
}


void loop(){
  
  /*  if(first){
        startingTime = millis();
    }*/
    
    
    if(!motor.commandHandled){
         int wheelSpeed = motorSpeed.computeSpeed(motor.getAngle());
         motor.setSpeed(wheelSpeed);
         double tempSetpoint = abs(wheelSpeed * 180.0 / 255.0);
         motor.setSetpoint(tempSetpoint);
         int distance = motor.distanceInTicks(motor.getMagnitude()); 
         double distanceScalar = motorSpeed.ticksPercentage(motor.getAngle(), distance);
         motor.checkEndpointReached(distanceScalar);
    }
         
        if(motor.getSpeed() < 0){
            motor.backwards();
        }else if (motor.getSpeed() > 0){
            motor.forward();
        }else{
            motor.stopMotors();
        }
     
    //motor.diffTicks();
    
    //Serial.print("DistanceScaler: ");   
    //Serial.println(distanceScalar);
    

    //runs PID - needs to run every loop()
    //Serial.println(motor.isFinished());
    if(!motor.isFinished() && !motor.commandHandled){
        //Serial.println("PID Running");
        motor.runPID();
    }else{
        timePassed = millis() - startingTime;
        //Serial.println("Time taken: ");
        //Serial.println(timePassed);
    }
}

