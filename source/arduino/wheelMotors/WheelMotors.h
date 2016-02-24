#ifndef WHEELMOTORS_H
#define WHEELMOTORS_H

class WheelMotors{

	public:
		static void printInfo();
		static void reset();
		static void resetAll();
		static void stopMotors();
		static void checkSetpointReached();
		static void runPID();
		static void diffTicks();
		static int distanceInTicks();
		static void M1_EncoderISR();
		static void M2_EncoderISR();
		static void moveCommandHandler(moveCommand command);
		
	private:

		volatile unsigned long M1_EncoderCount;
		volatile unsigned long M2_EncoderCount;
		static double Setpoint, M1_Input, M2_Input, M1_Output, M2_Output;
		static bool finished;
		int M1_Total;
		int M2_Total;
}
#endif
