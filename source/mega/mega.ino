#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include <ADXL345.h>
#include <QueueList.h>
#include <HMC5883L.h>
#include <Shared_Structs.h>
#include "Wire.h"
#include "SPI_Wrapper.h"
#include "I2C_Wrapper.h"
#include "Compass.h"
#include "Shared_Structs.h"
#include "TapDetectionLib.h"


const int laserScannerID = 25;
const int motorID1 = 17;
const int motorID2 = 16;

boolean sendMoveCommand = false;
boolean sendScanCommand = false;
boolean sendStopCommand = false;
boolean sendMoveResponse = false;
boolean sendCompassHeading = false;

moveCommand queuedMoveCommand;
scanCommand queuedScanCommand;
stopCommand queuedStopCommand;
compassCommand queuedCompassCommand;
moveResponse queuedMoveResponse;

Compass compass;
TapDetectionLib accelerometer;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);
  SPI_Wrapper::registerCompassCommandHandler(&compassCommandHandler);

  I2C_Wrapper::init(Master, 15);
  I2C_Wrapper::registerMoveResponseHandler(&moveResponseHandler);
  I2C_Wrapper::registerScanResponseHandler(&scanResponseHandler);
  I2C_Wrapper::registerStopCommandHandler(&stopCommandHandler);

  accelerometer = TapDetectionLib();
  accelerometer.init();

  compass = Compass();
  compass.init();

  Serial.println("Ready to receive");
}

void stopCommandHandler(stopCommand cmd) {
  //Serial.print("[Recieved Stop Command]");
  sendStopCommand = true;
  queuedStopCommand = cmd;
}

void moveCommandHandler(moveCommand cmd) {
  /*Serial.print("[Recieved Move Command]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude); */
  sendMoveCommand = true;
  queuedMoveCommand = cmd;
}

void moveResponseHandler(moveResponse cmd) {
  /*Serial.print("[Recieved Move Response]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude); */
  sendMoveResponse = true;
  queuedMoveResponse = cmd;
}

void scanCommandHandler(scanCommand cmd){
  //Serial.println("[Recieved Scan Command]");
  sendScanCommand = true;
  queuedScanCommand = cmd;
}

void compassCommandHandler(compassCommand cmd) {
  //Serial.println("[Recieved Compass Command]"); 
  sendCompassHeading = true;
  queuedCompassCommand = cmd;
}

void scanResponseHandler(scanResponse cmd) {
  /* Serial.print("[Recieved Scan Response]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude); */
  SPI_Wrapper::sendScanResponse(cmd.uniqueID, cmd.angle, cmd.magnitude, cmd.last, true);
}

  
void loop() {
  while (1) {
	  compass.updateHeading();

    if (accelerometer.checkIfTapped()) {
      Serial.println("Tapped");
      sendStopCommand = true;
    }
  
    if (sendMoveCommand) {
      //Serial.println("Preparing to send move command...");
      sendMoveCommand = false;
      I2C_Wrapper::sendMoveCommand(motorID1, queuedMoveCommand.angle, queuedMoveCommand.magnitude);
      I2C_Wrapper::sendMoveCommand(motorID2, queuedMoveCommand.angle, queuedMoveCommand.magnitude);
      //Serial.println("Sent move command..");
    }
    
    if (sendScanCommand) {
      //Serial.println("Preparing to send scan command...");
      sendScanCommand = false;
      I2C_Wrapper::sendScanCommand(laserScannerID);
    }

    if (sendStopCommand) {
      //Serial.println("Preparing to send stop command...");
      sendStopCommand = false;
      I2C_Wrapper::sendStopCommand(motorID1);
      I2C_Wrapper::sendStopCommand(motorID2);
    }
    
    if (sendMoveResponse) {
      //Serial.println("Preparing to send move response...");
      sendMoveResponse = false;
      SPI_Wrapper::sendMoveResponse(queuedMoveResponse.uniqueID, queuedMoveResponse.magnitude, queuedMoveResponse.angle, true);
    }
	
  	if (sendCompassHeading) {
  		//Serial.println("Sending compass heading...");
  		sendCompassHeading = false;
  		SPI_Wrapper::sendCompassResponse(queuedCompassCommand.uniqueID, compass.getHeading(), true);
  	}
  }
}
