#include <Arduino.h>
#include "Arduino.h"
#include <Compass.h>
void setup();
void loop();
#line 1 "src/test/test.ino"
//#include "Arduino.h"
//#include <Compass.h>

Compass a;

// Out setup routine, here we will configure the microcontroller and compass.
void setup() {
    // Initialize the serial port.
    Serial.begin(9600);

    Serial.println("Beginning setup()");
    a = Compass();
    a.init();
    Serial.println("Finishing setup()");
}


void loop() {
    a.updateHeading();
    Serial.println(a.getHeading());
}
