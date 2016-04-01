#ifndef WHEELMOTORS_H
#define WHEELMOTORS_H

#include "PID_v1.h"


class WheelMotors{

	public:
		static void setup();
		static void printInfo();
		static void forward();
		static void backwards();
		static void reset();
		static void resetAll();
		static void stop();
		static void checkSetpointReached();
		static void checkEndpointReached(int distance);
		static void runPID();
		static void start();
		static void diffTicks();
		static bool isFinished();
		static int distanceInTicks(int targetDistance);
		static moveCommand getMove();
		static uint16_t getMagnitude();
		static int getAngle();
		static int setSetpoint(int speed);
		static void stopMotors();
		static void moveCommandHandler(moveCommand command);
		static void stopCommandHandler(stopCommand command);
		static volatile unsigned long M1_EncoderCount, M2_EncoderCount;
		static bool finished, commandHandled;
		static PID myPID1,myPID2;
		static void setSpeed(double speed);
		static double getSpeed();

	private:


		static double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output, prev_M1_Speed, prev_M2_Speed, speed;
		static int M1_Total, M2_Total, D1, D2;
		static unsigned long timeSinceStart, timePassed, commandTimer, prevTimer, last100ms;
		static void M1_EncoderISR();
		static void M2_EncoderISR();
		//static PID myPID1;
		//static PID myPID2;
		static moveCommand currentMove;
};
#endif
