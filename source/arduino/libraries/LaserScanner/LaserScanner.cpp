#include "LaserScanner.h"

int LaserScanner::lastEncoderCount;
int LaserScanner::scanTick;
int LaserScanner::scanCount;
int LaserScanner::scansToDo;
int LaserScanner::scanFreq;
int LaserScanner::detectionAngle;
int LaserScanner::detectionRange;
int LaserScanner::averageRotations;
int LaserScanner::totalRotations;
bool LaserScanner::detectedDuringSpin;
bool LaserScanner::pushScanData;
String LaserScanner::scanType;
LaserReading* LaserScanner::lastRotationData;

LIDARLite LaserScanner::myLidarLite;

LaserScanner::LaserScanner(){
	detectedDuringSpin = false;
	pushScanData = false;
	scanType = "DEFAULT";
	scanTick = scanFreq = lastEncoderCount = totalRotations = averageRotations = totalRotations = 0;
	scansToDo = 1;
	lastRotationData = new LaserReading[scansToDo];
}

LaserScanner::~LaserScanner(){
	
}

void LaserScanner::sendScanResponse(LaserReading reading){
	bool lastScan = false;
	if (reading.distance >= 1061 || reading.distance < 0){
		reading.distance = 0;
	}
	if (scanTick == scansToDo){
		lastScan = true;
	}
	int angle = (360 / scansToDo) * reading.angle;
	//SPI_Wrapper::sendScanResponse(scanTick, (uint16_t) reading.distance, (uint16_t) angle, lastScan, true);
}

void LaserScanner::setup(){
	myLidarLite.begin();
	myLidarLite.beginContinuous();
}

// Sets ScanFreq in terms of encoder ticks (maximum of 'distance' per rotation) also sets the scan type. (DEFAULT, AVERAGE, INTERVAL)
void LaserScanner::setScanFreq(int freq, int distance, String type){
	scanType = type;
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

void LaserScanner::onMotorFinish(){
	totalRotations += 1;
}

void LaserScanner::getContinuousReading(int encoderCount){	
	int averageRotations = 3;
	if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % scanFreq == 0) {
		scanTick += 1;
		if (scanTick >= 0 && scanTick <= scansToDo){
			LaserReading reading = getSingleReading(scanTick-1);
			
			if (scanType == "DEFAULT") { 
				lastRotationData[scanTick-1] = reading;
				if (pushScanData){
					Serial.println("Sending Scan Data...");
					sendScanResponse(reading);
				}
			} else if (scanType == "AVERAGE") {
				if (totalRotations == 0){
					lastRotationData[scanTick-1] = LaserReading{0, 0};
				}
				if (reading.angle != -1 && reading.distance != -1){
					lastRotationData[scanTick-1].angle = lastRotationData[scanTick-1].angle + reading.angle;	
					lastRotationData[scanTick-1].distance = lastRotationData[scanTick-1].distance + reading.distance;	
				}
			} else if (scanType == "INTERVAL") {
					// Will be implemented shortly.
			} else {
				Serial.println("Unknown Scan Type...");
			}
			
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
	pushScanData = false;
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
