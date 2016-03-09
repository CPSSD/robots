#ifndef WHEELMOTORS_H
#define WHEELMOTORS_H

#include "PID_v1.h"

class WheelMotors{

	public:
		WheelMotors();
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
		static uint16_t getMagnitude();
		static void M1_EncoderISR();
		static void M2_EncoderISR();
		static void moveCommandHandler(moveCommand command);
		static volatile unsigned long M1_EncoderCount, M2_EncoderCount;
		static bool finished;

	private:


		static double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output;

		static int M1_Total, M2_Total, D1, D2, M1, M2;
		static PID myPID1;
		static PID myPID2;
		static moveCommand currentMove;
};
#endif
