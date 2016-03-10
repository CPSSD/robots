#include <Wire.h>
#include <I2C_Wrapper.h>

const byte slaveAddress = 27;
const int buttonPin = 7;

// Master
boolean sentCommand = false;

void moveResponseHandler(moveResponse response) {
  Serial.println(response.magnitude);
  sentCommand = false;
}

void setup() {
  Serial.begin(9600);
  I2C_Wrapper::init(Master, 0);
  I2C_Wrapper::addSlave(slaveAddress);
  I2C_Wrapper::registerMoveResponseHandler(&moveResponseHandler);
}

void loop() {
  I2C_Wrapper::stepI2C();

  if (digitalRead(buttonPin) == HIGH) {
    if (sentCommand == false) {
      sentCommand = true;
      I2C_Wrapper::sendMoveCommand(slaveAddress, 1000, 100);
    }
  }
}
