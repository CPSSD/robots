#include "LaserScanner.h"

int LaserScanner::lastEncoderCount;
int LaserScanner::scanTick;
int LaserScanner::scanCount;
LaserReading LaserScanner::lastRotationData[42];
LIDARLite LaserScanner::myLidarLite;

LaserScanner::LaserScanner(){
	lastEncoderCount = 0;
	scanTick = 0;
}

LaserScanner::~LaserScanner(){
	
}

void LaserScanner::setup(){
	myLidarLite.begin();
	myLidarLite.beginContinuous();
}

void LaserScanner::getContinuousReading(int encoderCount){	
	if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % 80 == 0) {
		scanTick += 1;
		if (scanTick >= 0 && scanTick <= 42){
			LaserReading reading = getSingleReading(scanTick-1);
			lastRotationData[scanTick-1] = reading;
			
			Serial.print("[");
			Serial.print(scanTick-1);
			Serial.print(" | ");
			Serial.print(lastRotationData[scanTick-1].distance);
			Serial.println("]");
			
			lastEncoderCount = encoderCount;
		}
	}
}

void LaserScanner::reset(){
	lastEncoderCount = 0;
	scanTick = 0;
}

LaserReading LaserScanner::getSingleReading(int angle){
	int distance = myLidarLite.distanceContinuous();
	
	if (distance > 1061 || distance < 0) {
		return LaserReading{angle, -1};
	} else {
		return LaserReading{angle, distance};
	}
}
