#include <PID_v1.h>

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

void doPID(){

}

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


int dist = 5000;
boolean finished = false;
int M1_Total = 0;
int M2_Total = 0;

int distance(int targetDistance){
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

void reset(/*boolean setpointReached*/){
    M1_EncoderCount = 0;
    M2_EncoderCount = 0;
    //setpointReached = false;
}


void stop(){
    analogWrite(M1,0);
    analogWrite(M2,0);
}

boolean start = true;

void setup()
{
    attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
    attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);
    
    myPID1.SetOutputLimits(100, 249);
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
    
   // reset(M1_EncoderCount,M2_EncoderCount);
    if(start){
        delay(2000);
        M1_EncoderCount = 0;
        M2_EncoderCount = 0;
        start = false;
    }
    
    
    if(M1_Total + M1_EncoderCount > dist && M2_Total + M2_EncoderCount > dist){
        finished = true;
        stop();
    }
    
    if(M1_EncoderCount > Setpoint && M2_EncoderCount > Setpoint){
        M1_Total += M1_EncoderCount;
        M2_Total += M2_EncoderCount;
        reset();
    }
    
    if(!finished){
        M1_Input = M1_EncoderCount;
        M2_Input = M2_EncoderCount;
        
        myPID1.Compute();
        myPID2.Compute();
        
        analogWrite(M1, M1_Output);
        analogWrite(M2, M2_Output);
        printInfo();
    }
}

