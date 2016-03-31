#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl;

void setup() {
    Serial.begin(9600);
    adxl.powerOn();

    // Look of tap movement on this axes - 1 == on; 0 == off.
    adxl.setTapDetectionOnX(1);
    adxl.setTapDetectionOnY(1);
    adxl.setTapDetectionOnZ(1);

    // Set values for what is a tap.
    adxl.setTapThreshold(25); // 62.5mg per increment.
    adxl.setTapDuration(15); // 625us per increment.

    // Setting all interrupts to take place on int pin 1.
    // I had issues with int pin 2, was unable to reset it.
    adxl.setInterruptMapping(ADXL345_INT_SINGLE_TAP_BIT, ADXL345_INT1_PIN);

    // Register interrupt actions - 1 == on; 0 == off.
    adxl.setInterrupt(ADXL345_INT_SINGLE_TAP_BIT, 1);
}

void loop() {
    byte interrupts = adxl.getInterruptSource();
    if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)) {
        Serial.println("tap!");
        // Add code here to do when a tap is sensed.
    }
}
