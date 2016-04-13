#include <Wire.h>
#include <TapDetectionLib>

TapDetectionLib tdl;

void setup() {
    Serial.begin(9600);
    tdl = TapDetectionLib();
    tdl.init();
    Serial.println("Setup() done.");
}

void loop() {
    if(tdl.checkIfTapped())
        Serial.println("tap!");
}
