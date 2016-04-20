#include "I2C_Wrapper.h"
#include "WheelMotors.h"
#include "Arduino.h"
#include "PID_v1.h"

#define M1_Interrupt 0
#define M2_Interrupt 1
#define circumference 31.4159265
#define ticksPerRevolution 800

volatile unsigned long WheelMotors::M1_EncoderCount = 0;
volatile unsigned long WheelMotors::M2_EncoderCount = 0;
int WheelMotors::M1_Total = 0;
int WheelMotors::M2_Total = 0;

int WheelMotors::D1 = 4;   // direction
int WheelMotors::D2 = 7;

const int M1 = 5;
const int M2 = 6;
const uint8_t masterId = 15;

unsigned long WheelMotors::timeSinceStart = millis();
unsigned long WheelMotors::commandTimer;
unsigned long WheelMotors::timePassed;
unsigned long WheelMotors::last100ms;
unsigned long WheelMotors::prevTimer = millis();

bool WheelMotors::finished = false;
bool WheelMotors::commandHandled = false;

double WheelMotors::Setpoint = 0;
double WheelMotors::M1_Input = 0;
double WheelMotors::M2_Input = 0;
double WheelMotors::M1_Output = 0;
double WheelMotors::prev_M1_Speed = 0;
double WheelMotors::prev_M2_Speed = 0;
double WheelMotors::M2_Output = 0;
moveCommand WheelMotors::currentMove;
PID WheelMotors::myPID1 = PID(&M1_Input, &M1_Output, &Setpoint, .3, 0, 0, DIRECT);
PID WheelMotors::myPID2 = PID(&M2_Input, &M2_Output, &Setpoint, .3, 0, 0, DIRECT);

double WheelMotors::speed = 0;

bool WheelMotors::isFinished(){
	return finished;
}

void WheelMotors::setup(){
	attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
	attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);

	myPID1.SetSampleTime(100);
	myPID1.SetMode(AUTOMATIC);
	myPID2.SetSampleTime(100);
	myPID2.SetMode(AUTOMATIC);

	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	digitalWrite(D1,HIGH);
	digitalWrite(D2,HIGH);
}

void WheelMotors::setSpeed(double newSpeed){
	speed = newSpeed;
}

double WheelMotors::getSpeed(){
	return speed;
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

int WheelMotors::setSetpoint(int wheelSpeed){
	Setpoint = (double) wheelSpeed;
	return Setpoint;
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

//stops wheels & sends stop response
void WheelMotors::stop(){
	analogWrite(M1,0);
	analogWrite(M2,0);
	timePassed = millis();
	Serial.print("Time taken: ");
	Serial.println(timePassed);

	I2C_Wrapper::sendMoveResponse(masterId,currentMove.uniqueID, currentMove.magnitude, currentMove.angle, true);
	//I2C_Wrapper::sendStopCommand(masterId);
	Serial.println("Stop command sent");
	commandHandled = true;
	resetAll();
}

// stops motors
void WheelMotors::stopMotors(){
	analogWrite(M1,0);
	analogWrite(M2,0);
}

//stops wheels once distance has been reached
void WheelMotors::checkEndpointReached(int distance){
	if(M1_Total + M1_EncoderCount > distance && M2_Total + M2_EncoderCount > distance){
		finished = true;
		stop();
		commandHandled = true;
	}
}

void WheelMotors::addToTotalCount(unsigned long M1, unsigned long M2){
	M1_Total += M1;
	M2_Total += M2;
}

// runs PID
void WheelMotors::runPID(){
	last100ms = millis();

	if(last100ms - prevTimer > 100){
		prevTimer = last100ms;
		Serial.print("Setpoint: "); Serial.println(Setpoint);

		M1_Input = M1_EncoderCount;
		M2_Input = M2_EncoderCount;
		addToTotalCount(M1_EncoderCount,M2_EncoderCount);
		reset();

		myPID1.Compute();
		myPID2.Compute();

		//Negate readings if going too fast
		if(M1_Input > Setpoint){
			M1_Output = (Setpoint - M1_Input) * .3;
		}
		if(M2_Input > Setpoint){
			M2_Output = (Setpoint - M2_Input) * .3;
		}

		//set new Speeds
		double M1_Speed = ((M1_Output + prev_M1_Speed) * 255) / 180;
		double M2_Speed = ((M2_Output + prev_M2_Speed) * 255) / 180;

		//log previous speed for next loop
		prev_M1_Speed = (M1_Speed * 180) / 255;
		prev_M2_Speed = (M2_Speed * 180) / 255;

		//setting max as 255
		if(M1_Speed > 255) M1_Speed = 255;
		if(M2_Speed > 255) M2_Speed = 255;

		analogWrite(M1,M1_Speed);
		analogWrite(M2,M2_Speed);
		//printInfo();
	}
}

void WheelMotors::stopCommandHandler(stopCommand command){
	Serial.println("Stop command recieved");
	commandHandled = true;
	stopMotors();
}

void WheelMotors::moveCommandHandler(moveCommand command){
	commandHandled = false;
	resetAll();
	Serial.println("Recieved");
	Serial.println(command.magnitude);
	Serial.println(command.angle);
	//I2C_Wrapper::sendStopCommand(masterId);
	commandTimer = millis() - timeSinceStart;
	currentMove = command;

	if(finished == true){
		//currentMove = command;
		finished = false;
		//commandHandled = true;
	}
}


void WheelMotors::printInfo(){
	Serial.println();
	Serial.println();

	Serial.println(Setpoint);
	Serial.print("Motor 1 Input: ");
	Serial.println(M1_Input);
	Serial.print("Motor 2 Input: ");
	Serial.println(M2_Input);
    Serial.print("Motor 1 Output: ");
    Serial.println(M1_Output);
	Serial.print("Motor 2 Output: ");
	Serial.println(M2_Output);
	Serial.print("Motor 1 EncoderCount: ");
	Serial.println(M1_EncoderCount);
	Serial.print("Motor 2 EncoderCount: ");
	Serial.println(M2_EncoderCount);
	Serial.print("Motor 1 Total Encoder count: ");
    Serial.println(M1_Total);
	Serial.print("Motor 2 Total Encoder count: ");
    Serial.println(M2_Total);
	Serial.println("End of Info");

    Serial.println();
	Serial.println();
}
