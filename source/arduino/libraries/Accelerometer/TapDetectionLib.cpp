#include <TapDetectionLib.h>

// Inits the accelerometer.
void TapDetectionLib::init() {
    adxl = ADXL345();
    adxl.powerOn();

    // Look of tap movement on this axes - 1 == on; 0 == off.
    adxl.setTapDetectionOnX(1);
    adxl.setTapDetectionOnY(1);
    adxl.setTapDetectionOnZ(1);

    // Set values for what is a tap.
    adxl.setTapThreshold(100); // 62.5mg per increment.
    adxl.setTapDuration(15); // 625us per increment.

    // Setting all interrupts to take place on int pin 1.
    adxl.setInterruptMapping(ADXL345_INT_SINGLE_TAP_BIT, ADXL345_INT1_PIN);

    // Register interrupt actions - 1 == on; 0 == off.
    adxl.setInterrupt(ADXL345_INT_SINGLE_TAP_BIT, 1);
}

// Returns true if the single tap interrupt has been triggered.
bool TapDetectionLib::checkIfTapped() {
    byte interrupts = adxl.getInterruptSource();
    return adxl.triggered(interrupts, ADXL345_SINGLE_TAP);
}
