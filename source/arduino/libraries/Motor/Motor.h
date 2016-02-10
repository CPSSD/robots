#ifndef Motor_H
#define Motor_H

#include <Arduino.h>

typedef void (*Rotation_Function)(int);

class Motor {
	public:
		Motor();
		~Motor();
		
		void changeDirection();
		void rotateClockwise();
		void rotateAnticlockwise();
		void startRotate();
		void stopRotate();
		void setup();
		void oneRotation(int fullSpin);
		
		void registerRotationFunction(Rotation_Function function);
		
		int numTicks(int offset);
		int correctTicks(int correction);
		int rotateWithCorrection(int rotationAmmount);
		int rotateByAngle(int angle);

		static volatile unsigned long encoderCount;
		volatile unsigned int completedOneRevolution;
		const int interupt = 0;
		const int speed = 200;
		const int singleRotation = 3360;
		
	private:
		const int E1 = 5;
		const int M1 = 4;
		const int E2 = 6;
		const int M2 = 7;
		
		bool anticlockwise;
		int correction;
		
		Rotation_Function rotationFunction;
};

#endif