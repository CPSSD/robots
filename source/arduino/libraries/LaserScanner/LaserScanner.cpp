#include "LaserScanner.h"

int LaserScanner::lastEncoderCount;
int LaserScanner::scanTick;
int LaserScanner::scanCount;
int LaserScanner::scansToDo;
int LaserScanner::scanFreq;
int LaserScanner::detectAngleStart;
int LaserScanner::detectAngleEnd;
int LaserScanner::detectionRange;
int LaserScanner::queuedRotations;
int LaserScanner::totalRotations;
int LaserScanner::scanOffset;

bool LaserScanner::detectedDuringSpin;
bool LaserScanner::pushScanData;

String LaserScanner::scanType;
LaserReading* LaserScanner::lastRotationData;

LIDARLite LaserScanner::myLidarLite;

LaserScanner::LaserScanner(){
	detectedDuringSpin = false;
	pushScanData = false;
	scanType = "DEFAULT";
	scanTick = scanFreq = lastEncoderCount = totalRotations = queuedRotations = totalRotations = scanOffset = 0;
	scansToDo = 1;
	lastRotationData = new LaserReading[scansToDo];
}

LaserScanner::~LaserScanner(){
	
}

void LaserScanner::sendScanResponse(LaserReading reading){
	bool lastScan = false;
	if (reading.angle == -1 || reading.distance == -1 || reading.distance >= 1060){
		return;
	}
	if (scanTick == scansToDo){
		lastScan = true;
	}
	int angle = tickToDegrees(reading.angle);
	Serial.print("Angle: ");
	Serial.println(angle);
	// uint8_t masterId, uint16_t uniqueID, uint16_t angle, uint16_t magnitude, bool last, bool status
	I2C_Wrapper::sendScanResponse((uint8_t) 15, 0, (uint16_t) angle, (uint16_t) reading.distance, lastScan, true);
}

void LaserScanner::setup(){
	myLidarLite.begin();
	myLidarLite.beginContinuous();
}

// Sets ScanFreq in terms of encoder ticks (maximum of 'distance' per rotation) also sets the scan type. (DEFAULT, AVERAGE, INTERVAL)
void LaserScanner::setScanFreq(int freq, int distance, String type, int rotations){
	scanType = type;
	queuedRotations = rotations;
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

void LaserScanner::setScanOffset(int offset){
	scanOffset = offset;
}

void LaserScanner::setDetectionAngle(int startAngle, int endAngle){
	if (startAngle < 0){
		detectAngleStart = 0;
	} else if (startAngle >= 3360){
		detectAngleStart = 3360;
	} else {
		detectAngleStart = startAngle;
	}
	
	if (endAngle < 0){
		detectAngleEnd = 0;
	} else if (endAngle >= 3360){
		detectAngleEnd = 3360;
	} else {
		detectAngleEnd = endAngle;
	}
}

void LaserScanner::setDetectionRange(int range){
	detectionRange = range;
}

int LaserScanner::tickToDegrees(int angle) {
	return (360.0 / (float) scansToDo) * (float) angle;
}

void LaserScanner::sendDetectResponse(int angle, int distance) {
	I2C_Wrapper::sendDetectResponse(15, 0, (uint16_t) angle, (uint32_t) distance, true);
}

void LaserScanner::detectObjects(int encoderCount){	
	if (encoderCount != lastEncoderCount && (encoderCount >= detectAngleStart && encoderCount <= detectAngleEnd)){
		LaserReading reading = getSingleReading(encoderCount);
		Serial.print(reading.angle);
		Serial.print(": Distance away: ");
		Serial.println(reading.distance);
		
		if (reading.distance <= -1 || reading.distance >= 1000){
			lastEncoderCount = encoderCount;
			return;
		}
		
		if (reading.distance <= detectionRange){
			Serial.println("-> Found object! Be careful!");
			int degree = ((float)reading.angle / 3360.0) * 360.0;
			Serial.println(degree);
			sendDetectResponse(degree, reading.distance);
		}
		
		lastEncoderCount = encoderCount;
	}
}

void LaserScanner::onMotorFinish(){
	totalRotations += 1;
}

void LaserScanner::setDetectionParameters(int startAngle, int endAngle, int distance){
	setDetectionRange(distance);
	setDetectionAngle(startAngle, endAngle);
}

void LaserScanner::getContinuousReading(int encoderCount){	
	if (lastEncoderCount != encoderCount && encoderCount > 0 && encoderCount % (scanFreq + scanOffset) == 0) {
		scanTick += 1;
		if (scanTick >= 0 && scanTick <= scansToDo){
			LaserReading reading = getSingleReading(scanTick-1);
			
			if (reading.distance >= 1000){
				reading.distance = -1;
				reading.angle = -1;
			}
			
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
				if ((scanTick - scanOffset) % queuedRotations == 0) {
					Serial.println(scanTick);
					lastRotationData[scanTick-1] = reading;
				} else {
					lastRotationData[scanTick-1] = LaserReading{-1, -1};
				}
			} else {
				Serial.println("Unknown Scan Type...");
			}
			
			if (scanType == "INTERVAL" && (scanTick - scanOffset) % queuedRotations == 0) {
				Serial.print("[");
				Serial.print(scanTick-1);
				Serial.print(" | ");
				Serial.print(lastRotationData[scanTick-1].distance);
				Serial.println("]");
			} else if (scanType != "INTERVAL"){
				Serial.print("[");
				Serial.print(scanTick-1);
				Serial.print(" | ");
				Serial.print(lastRotationData[scanTick-1].distance);
				Serial.println("]");
			}
			
			lastEncoderCount = encoderCount;
		}
	}
}

void LaserScanner::reset(){
	lastEncoderCount = 0;
	scanOffset = 0;
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
