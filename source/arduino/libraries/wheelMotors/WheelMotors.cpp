#include "I2C_Wrapper.h"
#include "WheelMotors.h"
#include "Arduino.h"
#include "PID_v1.h"
#include "Time.h"

#define M1_Interrupt 0
#define M2_Interrupt 1
#define circumference 31.4159265
#define ticksPerRevolution 800

volatile unsigned long WheelMotors::M1_EncoderCount = 0;
volatile unsigned long WheelMotors::M2_EncoderCount = 0;
int WheelMotors::M1_Total = 0;
int WheelMotors::M2_Total = 0;

int WheelMotors::D1 = 4;   // direction
int WheelMotors::M1 = 5;   // PWM
int WheelMotors::D2 = 7;
int WheelMotors::M2 = 6;

const uint8_t masterId = 15;

time_t WheelMotors::t = now();

bool WheelMotors::finished = true;
bool WheelMotors::commandHandled = false;

double WheelMotors::Setpoint = 400;
double WheelMotors::M1_Input = 0;
double WheelMotors::M2_Input = 0;
double WheelMotors::M1_Output = 0;
double WheelMotors::M2_Output = 0;
moveCommand WheelMotors::currentMove;
PID WheelMotors::myPID1 = PID(&M1_Input, &M1_Output, &Setpoint, 1, 10, 0, DIRECT);
PID WheelMotors::myPID2 = PID(&M2_Input, &M2_Output, &Setpoint, 1, 10, 0, DIRECT);

double WheelMotors::speed;

bool WheelMotors::isFinished(){
		if(finished){
				return true;
		}
		return false;
}

void WheelMotors::setup(){
		attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
		attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);

		myPID1.SetOutputLimits(100,255);
		myPID1.SetSampleTime(10);
		myPID1.SetMode(AUTOMATIC);
		myPID2.SetOutputLimits(100,255);
		myPID2.SetSampleTime(10);
		myPID2.SetMode(AUTOMATIC);

		pinMode(D1, OUTPUT);
		pinMode(D2, OUTPUT);
		digitalWrite(D1,HIGH);
		digitalWrite(D2,HIGH);
}

moveCommand WheelMotors::getMove(){
		return currentMove;
}

uint16_t WheelMotors::getMagnitude(){
		return currentMove.magnitude;
}

int WheelMotors::getAngle(){
		return currentMove.angle;
}

void WheelMotors::forward(){
		digitalWrite(D1,HIGH);
		digitalWrite(D2,HIGH);
}

void WheelMotors::backwards(){
		digitalWrite(D1,LOW);
		digitalWrite(D2,LOW);
}

void WheelMotors::M1_EncoderISR(){
	M1_EncoderCount++;
}

void WheelMotors::M2_EncoderISR(){
	M2_EncoderCount++;
}

int WheelMotors::distanceInTicks(int targetDistance){
	return (targetDistance/circumference)*ticksPerRevolution;
}


// resets M1,M2 counts for setpoint
void WheelMotors::reset(){
	M1_EncoderCount = 0;
	M2_EncoderCount = 0;
}

//resets all encoder counts
void WheelMotors::resetAll(){
	reset();
	M1_Total = 0;
	M2_Total = 0;
}

//used for testing
void WheelMotors::start(){
	analogWrite(M1,255);
	analogWrite(M2,255);
}


double WheelMotors::timePast(time_t time){
	return time - t;
}

//stops wheels
void WheelMotors::stop(){
	analogWrite(M1,0);
	analogWrite(M2,0);

	I2C_Wrapper::sendMoveResponse(masterId,currentMove.uniqueID, currentMove.magnitude, currentMove.angle, true);
	commandHandled = false;
	resetAll();
}


void WheelMotors::checkEndpointReached(int distance){
	if(M1_Total + M1_EncoderCount > distance && M2_Total + M2_EncoderCount > distance){
		finished = true;
		stop();
	}
}

void WheelMotors::checkSetpointReached(){
	if(M1_EncoderCount > Setpoint && M2_EncoderCount > Setpoint){
		M1_Total += M1_EncoderCount;
		M2_Total += M2_EncoderCount;
		reset();
	}
}

void WheelMotors::runPID(){
	M1_Input = M1_EncoderCount;
	M2_Input = M2_EncoderCount;

	myPID1.Compute();
	myPID2.Compute();

	analogWrite(M1, M1_Output);
	analogWrite(M2, M2_Output);
	//printInfo();
}

void WheelMotors::diffTicks(){
    if ((M1_Total + M1_EncoderCount) > (M2_Total + M2_EncoderCount) && (M1_Total + M1_EncoderCount) - (M2_Total + M2_EncoderCount) > 50){
        // if M1 gets bigger than M2, lower M1 max speed and increase M2's min speed
				myPID1.SetOutputLimits(100,speed);
        myPID2.SetOutputLimits(150,speed);
				//Serial.println("M1 Bigger");
    }else if((M1_Total + M1_EncoderCount) < (M2_Total + M2_EncoderCount) && (M2_Total + M2_EncoderCount) - (M1_Total + M1_EncoderCount) > 50){
        // if M2 gets bigger than M1, lower M2 max speed and increase M1's min speed
        myPID2.SetOutputLimits(100,speed);
        myPID1.SetOutputLimits(150,speed);
				//Serial.println("M2 Bigger");
    }else{
        myPID1.SetOutputLimits(0,speed);
        myPID2.SetOutputLimits(0,speed);
				//Serial.println(M1_Total + M1_EncoderCount);
				//Serial.println(M2_Total + M2_EncoderCount);
        //Serial.println("M1 = M2");
    }
}

void WheelMotors::moveCommandHandler(moveCommand command){
	Serial.println("Recieved");
	Serial.println(command.magnitude);
	Serial.println(command.angle);


	if(finished == true){
		currentMove = command;
		finished = false;
		commandHandled = true;
	}
}


void WheelMotors::printInfo(){
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
