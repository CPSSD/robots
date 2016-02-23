#include <PID_v1.h>
#include <Wire.h>
#include <I2C_Wrapper.h>

const byte slaveAddress = 27;

int D1 = 4;   // direction
int M1 = 5;   // PWM
int D2 = 7;
int M2 = 6;

int M1_Interrupt = 0;
int M2_Interrupt = 1;

volatile unsigned long M1_EncoderCount = 0;
volatile unsigned long M2_EncoderCount = 0;


double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output;

PID myPID1(&M1_Input, &M1_Output, &Setpoint, .5, 0, 0, DIRECT);
PID myPID2(&M2_Input, &M2_Output, &Setpoint, .5, 0, 0, DIRECT);

//PID_ATune PIDTuner(&Input,&Output);

void M1_EncoderISR() {
  M1_EncoderCount++;
}

void M2_EncoderISR() {
  M2_EncoderCount++;
}

//int M1_Speed = 200; // Keep speed of M1 constant and adkust M2 as appropriate
//int M2_Speed = 200;

// wheels
float circumference = 31.4159265; // cm
int ticksPerRevolution = 800; 

moveCommand currentMove;

boolean finished = true;
int M1_Total = 0;
int M2_Total = 0;

//returns number of ticks for required distance
int distanceInTicks(int targetDistance){
    return (targetDistance/circumference)*800;
}


void printInfo(){
    Serial.print("Motor 1 Output: ");
    Serial.println(M1_Output);
    Serial.print("Motor 2 Output: ");
    Serial.println(M2_Output);
    Serial.print("Motor 1 Encoder count: ");
    Serial.println(M1_EncoderCount);
    Serial.print("Motor 2 Encoder count: ");
    Serial.println(M2_EncoderCount);
    Serial.print("Total Motor 1 Ticks: ");
    Serial.println(M1_Total + M1_EncoderCount);
    Serial.print("Total Motor 2 Ticks: ");
    Serial.println(M2_Total + M2_EncoderCount);  
      
    Serial.println();
}

//sets Motor encoders to 0
void reset(){
    M1_EncoderCount = 0;
    M2_EncoderCount = 0;
}

void resetAll() {
    reset();
    M1_Total = 0;
    M2_Total = 0;
}

//stops motors
void stop(){
    analogWrite(M1,0);
    analogWrite(M2,0);

    I2C_Wrapper::sendMoveResponse(currentMove.uniqueID, currentMove.magnitude, currentMove.angle, true);

    resetAll();
}

//stop when desired number of ticks have been reached
void checkEndpointReached(int distance){
    if(M1_Total + M1_EncoderCount > distance && M2_Total + M2_EncoderCount > distance){
        finished = true;
        stop();
    }
}

//reset Motor encoders after setpoint ticks has been reached
void checkSetpointReached(){
    if(M1_EncoderCount > Setpoint && M2_EncoderCount > Setpoint){
        M1_Total += M1_EncoderCount;
        M2_Total += M2_EncoderCount;
        reset();
    }
}

//runs PID
void runPID(){
    M1_Input = M1_EncoderCount;
    M2_Input = M2_EncoderCount;
        
    myPID1.Compute();
    myPID2.Compute();
        
    analogWrite(M1, M1_Output);
    analogWrite(M2, M2_Output);
    printInfo();
}

void diffTicks(){
    if ((M1_Total + M1_EncoderCount) > (M2_Total + M2_EncoderCount) && (M1_Total + M1_EncoderCount) - (M2_Total + M2_EncoderCount) > 50){
        /*Serial.print("M1 Total + M1 EncoderCount: ");
        Serial.println(M1_Total + M1_EncoderCount);
        Serial.print("M2 Total + M1 EncoderCount: ");
        Serial.println(M2_Total + M2_EncoderCount);*/
        // if M1 gets bigger than M2, lower M1 max speed and increase M2's min speed  
        myPID1.SetOutputLimits(100,200);
        myPID2.SetOutputLimits(150,250);
        //Serial.println("M1 Higher");
    }else if((M1_Total + M1_EncoderCount) < (M2_Total + M2_EncoderCount) && (M2_Total + M2_EncoderCount) - (M1_Total + M1_EncoderCount) > 50){
        // if M2 gets bigger than M1, lower M2 max speed and increase M1's min speed
        myPID2.SetOutputLimits(100,200);
        myPID1.SetOutputLimits(150,250);
        //Serial.println("M2 Higher");
        
    }else{
        myPID1.SetOutputLimits(100,250);
        myPID2.SetOutputLimits(100,250);
        Serial.println("Here");
    }
}

void moveCommandHandler(moveCommand command) {
  Serial.println("Received command");
  Serial.println(command.magnitude);
  
  if (finished == true) {
    currentMove = command;
    finished = false;
  }
}

void setup()
{
    attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
    attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);

    I2C_Wrapper::init(Slave, slaveAddress);
    I2C_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
    currentMove.magnitude = 0;
    
    myPID1.SetOutputLimits(100, 250);
    myPID1.SetSampleTime(10);
    myPID2.SetOutputLimits(100, 250);
    myPID2.SetSampleTime(10);    
    myPID1.SetMode(AUTOMATIC);
    myPID2.SetMode(AUTOMATIC);
    
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);

    M1_Input = M1_EncoderCount; 
    M2_Input = M2_EncoderCount;
    Setpoint = 800;
    
    digitalWrite(D1,HIGH);
    digitalWrite(D2,HIGH);
    
    Serial.begin(9600);  
}


void loop(){
    
    //stop when desired number of ticks have been reached
    diffTicks();
    
    checkEndpointReached(distanceInTicks(currentMove.magnitude));
    
    //reset Motor encoders after setpoint ticks has been reached
    checkSetpointReached();
    
    //runs PID - needs to run ever loop()
    if(!finished){
        runPID();
    }
}

