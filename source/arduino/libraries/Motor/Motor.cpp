#include "Motor.h"

volatile unsigned long Motor::encoderCount = 0;

Motor::Motor(){
	completedOneRevolution = false;
	anticlockwise = true;
	correction = 0;
	speed = 200;
	rotationFunction = NULL;
}

Motor::~Motor(){
}

void Motor::setSpeed(int newSpeed){
	if(newSpeed <= 150){
		speed = 150;
	} else if (newSpeed >= 200){
		speed = 200;
	} else {
		speed = newSpeed;
	}
}

void encoderISR(){
    Motor::encoderCount++;
}

void Motor::rotateAnticlockwise(){
    anticlockwise = true;
    digitalWrite(M1,HIGH);
    digitalWrite(M2,HIGH);

}

void Motor::rotateClockwise(){
    anticlockwise = false;
    digitalWrite(M1,LOW);
    digitalWrite(M2,LOW);
}

void Motor::changeDirection(){
	Serial.print(anticlockwise);
    if (anticlockwise) {
        rotateClockwise();
    } else {
        rotateAnticlockwise();
    }
}

void Motor::startRotate(){
    analogWrite(E1,speed);
}

void Motor::stopRotate(){
    analogWrite(E1,0);
}

void Motor::setup(){
    attachInterrupt(interupt,&encoderISR,FALLING);
	rotateAnticlockwise();
}

int Motor::numTicks(int offset){
    int correction = encoderCount - offset;
    return correction;
}

int Motor::correctTicks(int correction){
    encoderCount = 0;
    startRotate();
	
    Serial.println("***** ");
	Serial.println(correction);
    
	while(encoderCount < correction){}

	stopRotate();
    delay(500);
    
	//Serial.println(encoderCount);
    return encoderCount - correction;
}

void Motor::registerRotationFunction(Rotation_Function function){
	rotationFunction = function;
}

void Motor::registerRotationFinishedFunction(Rotation_Finished_Function function){
	rotationFinishedFunction = function;
}

void Motor::rotateContinuous(int rotations){
	startRotate();
	while(rotations >= 0){
		if (encoderCount > singleRotation){
			rotations--;
			encoderCount -= singleRotation;
		}
			
		(*rotationFunction)(encoderCount);
	}
	stopRotate();
	(*rotationFinishedFunction)();
	
}

void Motor::oneRotation(int fullSpin){
	encoderCount = 0;
    startRotate();
    while(encoderCount < fullSpin){
		if (rotationFunction != NULL) {
			(*rotationFunction)(encoderCount);
		} else {
			Serial.println("You need to register a rotation function if you want stuff to happen...");
		}
	}
    stopRotate();
	(*rotationFinishedFunction)();
	delay(500);
}

// Rotates and then corrects its overspin. Returns the ammount of ticks the laser is from its target
int Motor::rotateWithCorrection(int rotationAmmount){
	int correction = 0; 
	
    oneRotation(rotationAmmount);
    correction = numTicks(rotationAmmount);
	
    changeDirection();
    correction = correctTicks(correction);
	changeDirection();
	return correction;
}

// Converts the angle into ticks and calls rotateWithCorrection(...);
int Motor::rotateByAngle(int angle){
	int rotationAmmount = ((double)(singleRotation / 360.0)) * angle;
	return rotateWithCorrection(rotationAmmount);
}
