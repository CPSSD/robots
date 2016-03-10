#include "SPI_Wrapper.h"
#include "I2C_Wrapper.h"

const int laserScannerID = 30;
const int motorID = 27;

boolean sendMoveCommand = false;
boolean sendScanCommand = false;
boolean sendMoveResponse = false;
boolean sendScanResponse = false;

moveCommand queuedMoveCommand;
scanCommand queuedScanCommand;
moveResponse queuedMoveResponse;
scanResponse queuedScanResponse;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  SPI_Wrapper::init();
  SPI_Wrapper::registerMoveCommandHandler(&moveCommandHandler);
  SPI_Wrapper::registerScanCommandHandler(&scanCommandHandler);

  I2C_Wrapper::init(Master, 15);
  I2C_Wrapper::registerMoveResponseHandler(&moveResponseHandler);
  I2C_Wrapper::registerScanResponseHandler(&scanResponseHandler);
  Serial.println("Ready to recieve.");
}

void moveCommandHandler(moveCommand cmd) {
  Serial.print("[Recieved Move Command]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude);
  sendMoveCommand = true;
  queuedMoveCommand = cmd;
}

void moveResponseHandler(moveResponse cmd) {
  Serial.print("[Recieved Move Response]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude);
  sendMoveResponse = true;
  queuedMoveResponse = cmd;
}

void scanCommandHandler(scanCommand cmd){
  Serial.println("[Recieved Scan Command]");
  sendScanCommand = true;
  queuedScanCommand = cmd;
}

void scanResponseHandler(scanResponse cmd) {
  Serial.print("[Recieved Scan Response]: ");
  Serial.print(cmd.angle);
  Serial.print(" / ");
  Serial.println(cmd.magnitude);
  sendScanResponse = true;
  queuedScanResponse = cmd;
}

  
void loop() {
  while (1) {
    if (sendMoveCommand) {
      Serial.println("Preparing to send move command...");
      sendMoveCommand = false;
      I2C_Wrapper::sendMoveCommand(motorID, queuedMoveCommand.angle, queuedMoveCommand.magnitude);
      Serial.println("Sent move command..");
    }
    
    if (sendScanCommand) {
      Serial.println("Preparing to send scan command...");
      sendScanCommand = false;
      I2C_Wrapper::sendScanCommand(laserScannerID);
    }
    
    if (sendMoveResponse) {
      Serial.println("Preparing to send move response...");
      sendScanResponse = false;
      SPI_Wrapper::sendMoveResponse(queuedMoveResponse.uniqueID, queuedMoveResponse.magnitude, queuedMoveResponse.angle, true);
    }
    
    if (sendScanResponse) {
      Serial.println("Preparing to send scan response...");
      sendScanResponse = false;
      SPI_Wrapper::sendScanResponse(queuedScanResponse.uniqueID, queuedScanResponse.angle, queuedScanResponse.magnitude, queuedScanResponse.last, true);
    }
  }
}
