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
		
		static void reset();
		static void setup();
		
		static int getReading();
		
		static void getContinuousReading(int encoderCount);
		static LaserReading getSingleReading(int angle);
		
		static LaserReading lastRotationData[42];
		static LIDARLite myLidarLite;
		
		static int lastEncoderCount;
		static int scanCount;
		static int scanTick;
};

#endif