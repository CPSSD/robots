#include <PID_v1.h>
#include <Wire.h>
#include <I2C_Wrapper.h>
#include <VectorMath.h>
#include <Time.h>
#include <Shared_Structs.h>
#include <WheelMotors.h>

const byte slaveAddress = 27;

WheelMotors motor = WheelMotors();
VectorMath motorSpeed = VectorMath();

unsigned long t1,t2;

boolean first;

void setup()
{
    I2C_Wrapper::init(Slave, slaveAddress);
    I2C_Wrapper::registerMoveCommandHandler(&motor.moveCommandHandler);
   // currentMove.magnitude = 0;
    
    motor.setup();
    
    motor.finished = true;
    first = true;
    //motor.start();
    Serial.begin(9600);  
}


void loop(){
   
     if(first){
       t1 = millis();
       first = false;
     }
     double newSpeed;
     
     if (motor.commandHandled){
         newSpeed = motorSpeed.computeSpeed(/*motorSpeed.getSign(),*/motor.getAngle());
          
         motor.myPID1.SetOutputLimits(0,newSpeed); 
     }
     

     if(newSpeed < 0){
         motor.backwards();
         //digitalWrite(motor.D1,LOW);
         //digitalWrite(motor.D2,LOW);
     }else if (newSpeed > 0){
         motor.forward();
         //digitalWrite(motor.D1,HIGH);
         //digitalWrite(motor.D2,HIGH);
     }else{
         analogWrite(motor.M1,0);
         analogWrite(motor.M2,0);
         //Serial.println(newSpeed);
     }
     
     motor.diffTicks();
     //motor.printInfo();
        
     motor.checkEndpointReached(motor.distanceInTicks(motor.getMagnitude()));
    
    //reset Motor encoders after setpoint ticks has been reached
     motor.checkSetpointReached();
    //Serial.println(speed1.computeSpeed(true,60));
    //delay(10000);
    //runs PID - needs to run ever loop()
    if(!motor.isFinished() && motor.getAngle() != 135 && motor.getAngle() != 270){
        motor.runPID();
    }else{
        
        t2 = millis() - t1;
       /* Serial.println();
	Serial.print("TotalTime: ");
        Serial.println(t2);
	Serial.println();*/
    }
    
}

