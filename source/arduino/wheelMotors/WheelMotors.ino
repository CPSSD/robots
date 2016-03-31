#include <PID_v1.h>
#include <Wire.h>
#include <I2C_Wrapper.h>
#include <VectorMath.h>
#include <Shared_Structs.h>
#include <WheelMotors.h>

const byte slaveAddress = 27;

WheelMotors motor = WheelMotors();
VectorMath motorSpeed = VectorMath();

unsigned long startingTime,timePassed;

boolean first = true;

void setup(){
    I2C_Wrapper::init(Slave, slaveAddress);
    I2C_Wrapper::registerMoveCommandHandler(&motor.moveCommandHandler);
    
    motor.setup();
    
    motor.commandHandled = false;
    motor.finished = false;
    //first = true;

    Serial.begin(9600);  
}


void loop(){
  
    if(first){
        startingTime = millis();
    }
    
    
    if(!motor.commandHandled){
         int wheelSpeed = motorSpeed.computeSpeed(motor.getAngle());
         //motor.setSpeed(motorSpeed.computeSpeed(/*motor.getAngle()*/0));
         Serial.print("Look: "); Serial.println(motor.getAngle());
         motor.setSpeed(wheelSpeed);
         motor.setSetpoint((wheelSpeed * 180) / 255);
         Serial.print("Speed: "); 
         Serial.println(motor.getSpeed());
         //motor.commandHandled = true;
    }
         
    if(first){
        if(motor.getSpeed() < 0){
            motor.backwards();
        }else if (motor.getSpeed() > 0){
            motor.forward();
        }else{
            motor.stopMotors();
        }
        first = false;
    }
     
    //motor.diffTicks();
        
    motor.checkEndpointReached(motor.distanceInTicks(motor.getMagnitude()));

    //runs PID - needs to run every loop()
    //Serial.println(motor.isFinished());
    if(!motor.isFinished()){
        //Serial.println("PID Running");
        motor.runPID();
    }else{
        timePassed = millis() - startingTime;
        //Serial.println("Time taken: ");
        //Serial.println(timePassed);
    }
}

