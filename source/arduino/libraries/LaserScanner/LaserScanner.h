#ifndef LaserScanner_H
#define LaserScanner_H

#include <Arduino.h>
#include "../LIDARLite/LIDARLite.h"
#include "../I2C/I2C_Wrapper.h"

struct LaserReading {
	int angle;
	int distance;
};

class LaserScanner {
	public:
		LaserScanner();
		~LaserScanner();
		
		static void setScanFreq(int freq, int distance, String type);
		static void setDetectionAngle(int encoderCount);
		static void setDetectionRange(int range);
		static void sendScanResponse(LaserReading reading);
		static void onMotorFinish();
		
		static void reset();
		static void setup();
		
		static int getReading();
		
		static void getContinuousReading(int encoderCount);
		static LaserReading getSingleReading(int angle);
		static void detectObjects(int encoderCount);
		
		static LaserReading* lastRotationData;
		static LIDARLite myLidarLite;
			
		static int lastEncoderCount;
		static int scansToDo;
		static int scanCount;
		static int scanTick;
		static int scanFreq;
		static int detectionAngle;
		static int detectionRange;
		static int averageRotations;
		static int totalRotations;
		static bool detectedDuringSpin;
		static bool pushScanData;
		static String scanType;
};

#endif