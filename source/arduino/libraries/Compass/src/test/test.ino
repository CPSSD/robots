#include <Arduino.h>
#include <Wire.h>
#include "Compass.h"

Compass compass;

// Out setup routine, here we will configure the microcontroller and compass.
void setup() {
    // Initialize the serial port.
    Serial.begin(9600);

    Serial.println("Beginning setup()");
    compass = Compass();
    Serial.println("Finishing setup()");
}


void loop() {
    compass.updateHeading();
    Serial.println(compass.getHeading());
}
