#include "Compass.h"

// Functions as a setup routine. Gets the compass object.
Compass::Compass() {
    Wire.begin(); // Start the I2C interface.
    compass.setScale(((float)0.88)); // Set the scale of the compass.
    compass.setMeasurementMode(MEASUREMENT_CONTINUOUS); // Set the measurement mode to Continuous
}

// Destructor. Included for completeness.
Compass::~Compass() {

}


// Called in the main loop to keep the compass heading up to date.
void Compass::updateHeading() {
    // Retrived the scaled values from the compass (scaled to the configured scale).
    MagnetometerScaled scaled = compass.readScaledAxis();

    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    float heading = atan2(scaled.YAxis, scaled.XAxis);

    // Correct for when signs are reversed.
    if(heading < 0)
        heading += 2*PI;

    // Check for wrap due to addition of declination.
    if(heading > 2*PI)
        heading -= 2*PI;

    // Convert radians to degrees for readability.
    float headingDegrees = heading * 180/M_PI;

    lastHeadings[headingsIndex++] = headingDegrees;
    headingsIndex %= MAX_HEADINGS;
}

// Averages the last MAX_HEADINGS compass readings and outputs them.
float Compass::getHeading() {
    float headingsSum = 0;
    for(int i=0; i<MAX_HEADINGS; i++)
        headingsSum += lastHeadings[i];

    return headingsSum/MAX_HEADINGS;
}
