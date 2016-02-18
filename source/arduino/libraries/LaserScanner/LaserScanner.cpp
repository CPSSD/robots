#include "LaserScanner.h"

int LaserScanner::lastEncoderCount;
int LaserScanner::scanTick;
int LaserScanner::scanCount;
int LaserScanner::scansToDo;
int LaserScanner::scanFreq;
int LaserScanner::detectionAngle;
int LaserScanner::detectionRange;
bool LaserScanner::detectedDuringSpin;
LaserReading* LaserScanner::lastRotationData;
LIDARLite LaserScanner::myLidarLite;

LaserScanner::LaserScanner(){
	lastEncoderCount = 0;
	detectedDuringSpin = false;
	scanTick = 0;
	scanFreq = 0;
	scansToDo = 1;
	lastRotationData = new LaserReading[scansToDo];
}

LaserScanner::~LaserScanner(){
	
}

void LaserScanner::setup(){
	myLidarLite.begin();
	myLidarLite.beginContinuous();
}

// Sets ScanFreq in terms of encoder ticks (maximum of 'distance' per rotation)
void LaserScanner::setScanFreq(int freq, int distance){
	if (freq <= 0){
		scanFreq = 0;
		scansToDo = 1;
	} else if (freq >= distance){
		scanFreq = distance;
		scansToDo = 1;
	} else {
		scanFreq = freq;
		scansToDo = distance/freq;
	}
	delete lastRotationData;
	lastRotationData = new LaserReading[scansToDo];
}

void LaserScanner::setDetectionAngle(int angle){
	if (angle < 0){
		detectionAngle = 0;
	} else if (angle >= 3360){
		detectionAngle = 3360;
	} else {
		detectionAngle = angle;
	}
}

void LaserScanner::setDetectionRange(int range){
	detectionRange = range;
}


void LaserScanner::detectObjects(int encoderCount){
	if (encoderCount < detectionAngle) {
		detectedDuringSpin = false;
	}
	
	if (encoderCount != lastEncoderCount && (encoderCount >= detectionAngle - 10 && encoderCount <= detectionAngle + 10) && !detectedDuringSpin){
		Serial.println("At correct angle....");
		LaserReading reading = getSingleReading(encoderCount);
		Serial.print("Distance away: ");
		Serial.println(reading.distance);
		if (reading.distance <= detectionRange){
			Serial.println("-> Found object! Be careful!");
		}
			
		Serial.print("[");
		Serial.print(encoderCount);
		Serial.print(" | ");
		Serial.print(reading.distance);
		Serial.println("]");
		
		lastEncoderCount = encoderCount;
	}
}

void LaserScanner::getContinuousReading(int encoderCount){	
	if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % scanFreq == 0) {
		scanTick += 1;
		if (scanTick >= 0 && scanTick <= scansToDo){
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
	
	if (distance > 1061 || distance < 5) {
		return LaserReading{angle, -1};
	} else {
		return LaserReading{angle, distance};
	}
}
