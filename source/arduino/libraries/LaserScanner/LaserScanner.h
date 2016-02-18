#ifndef LaserScanner_H
#define LaserScanner_H

#include <Arduino.h>
#include "../LIDARLite/LIDARLite.h"

struct LaserReading {
	int angle;
	int distance;
};

class LaserScanner {
	public:
		LaserScanner();
		~LaserScanner();
		
		static void setScanFreq(int freq, int distance);
		static void setDetectionAngle(int encoderCount);
		static void setDetectionRange(int range);
		
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
		static bool detectedDuringSpin;
};

#endif