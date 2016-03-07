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
		static void diffTicks();
		static int distanceInTicks(int targetDistance);
		static void M1_EncoderISR();
		static void M2_EncoderISR();
		static void moveCommandHandler(moveCommand command);

	private:

		static volatile unsigned long M1_EncoderCount, M2_EncoderCount;
		static double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output;
		static bool finished;
		static int M1_Total, M2_Total, D1, D2;
		static PID myPID1;
		static PID myPID2;
		static moveCommand currentMove;
};
#endif
