#ifndef WHEELMOTORS_H
#define WHEELMOTORS_H

#include "PID_v1.h"
#include "Time.h"

class WheelMotors{

	public:
		static void setup();
		static void printInfo();
		static void forward();
		static void backwards();
		static void reset();
		static void resetAll();
		static double timePast(time_t time);
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
		static void M1_EncoderISR();
		static void M2_EncoderISR();
		static void moveCommandHandler(moveCommand command);
		static volatile unsigned long M1_EncoderCount, M2_EncoderCount;
		static bool finished, commandHandled;
		static int M1, M2;
		static PID myPID1,myPID2;

	private:


		static double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output;
		static int M1_Total, M2_Total, D1, D2;
		static time_t t;
		static double speed;
		//static PID myPID1;
		//static PID myPID2;
		static moveCommand currentMove;
};
#endif
