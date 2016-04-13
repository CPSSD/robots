#ifndef LaserScanner_H
#define LaserScanner_H

#include <Arduino.h>
#include "../LIDARLite/LIDARLite.h"
#include "../I2C/I2C_Wrapper.h"

struct LaserReading {
	int angle;
	int distance;
};


typedef enum {
	Default = 0,
	Average = 1,
	Interval = 2
} ScanType;

class LaserScanner {
	public:
		LaserScanner();
		~LaserScanner();
		
		static void setScanFreq(int freq, int distance, ScanType type, int queuedRotations);
		static void setDetectionAngle(int startAngle, int endAngle);
		static void setDetectionRange(int range);
		static void sendScanResponse(LaserReading reading, bool lastScan);
		static void onMotorFinish();
		
		static void setDetectionParameters(int startAngle, int endAngle, int distance);
		static void sendDetectResponse(int angle, int distance);
		static int tickToDegrees(int angle);
		
		static void reset();
		static void setup();
		static void setScanOffset(int offset);
		
		static int getReading();
		
		static void getContinuousReading(int encoderCount);
		static LaserReading getSingleReading(int angle);
		static void detectObjects(int encoderCount);
		
		static LaserReading* lastRotationData;
		static LIDARLite myLidarLite;
		
		static bool pushScanData;
		static int scansToDo;
		static int totalRotations;
		static int queuedRotations;
	
	private:
		static int lastEncoderCount;
		static int scanCount;
		static int scanOffset;
		static int scanTick;
		static int scanFreq;
		static int detectAngleStart;
		static int detectAngleEnd;
		static int detectionRange;
		static bool detectedDuringSpin;
		static ScanType scanType;
};

#endif
