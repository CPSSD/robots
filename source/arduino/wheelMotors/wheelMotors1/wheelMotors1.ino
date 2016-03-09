#include <PID_v1.h>
#include <Wire.h>
#include <I2C_Wrapper.h>
#include <WheelMotors.h>

const byte slaveAddress = 27;

/////////////////////////////////////////////////////////////////

WheelMotors motor = WheelMotors();

//////////////////////////////////////////////////////////////////

//int M1_Speed = 200; // Keep speed of M1 constant and adkust M2 as appropriate
//int M2_Speed = 200;

// wheels


moveCommand currentMove;   /////////////////////////////////////////////// not included

void setup()
{
    //attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
    //attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);

    I2C_Wrapper::init(Slave, slaveAddress);
    //I2C_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
    currentMove.magnitude = 0;
    
    motor.setup();
    /*myPID1.SetOutputLimits(100, 250);
    myPID1.SetSampleTime(10);
    myPID2.SetOutputLimits(100, 250);
    myPID2.SetSampleTime(10);    
    myPID1.SetMode(AUTOMATIC);
    myPID2.SetMode(AUTOMATIC);
    
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    
    WheelMotors motor = new WheelMotors();
    motor.M1_Input = motor.M1_EncoderCount; 
    motor.M2_Input = motor.M2_EncoderCount;
    motor.Setpoint = 800;
    
    digitalWrite(D1,HIGH);
    digitalWrite(D2,HIGH);
    PID myPID1(&motor.M1_Input, &motor.M1_Output, &motor.Setpoint, .5, 0, 0, DIRECT);
    PID myPID2(&motor.M2_Input, &motor.M2_Output, &motor.Setpoint, .5, 0, 0, DIRECT);*/
    
    
    
    Serial.begin(9600);  
}


void loop(){
    
    //stop when desired number of ticks have been reached
    WheelMotors::diffTicks();
    
    WheelMotors::checkEndpointReached(WheelMotors::distanceInTicks(currentMove.magnitude));
    
    //reset Motor encoders after setpoint ticks has been reached
    WheelMotors::checkSetpointReached();
    
    //runs PID - needs to run ever loop()
    if(/*finished*/ 1){
        WheelMotors::runPID();
    }
}

