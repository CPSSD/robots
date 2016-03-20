#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L.h>

HMC5883L compass;

// Record any errors that may occur in the compass.
int error = 0;

// Define the number of headings to average over to reduce error.
#define MAX_HEADINGS 50
// Store last 10 readings.
float lastHeadings[MAX_HEADINGS] = {0};
// Stores our current position in lastHeadings.
int headingsIndex = 0;


// Out setup routine, here we will configure the microcontroller and compass.
void setup() {
    // Initialize the serial port.
    Serial.begin(9600);

    Serial.println("Starting the I2C interface.");
    Wire.begin(); // Start the I2C interface.

    Serial.println("Constructing new HMC5883L");

    Serial.println("Setting scale to +/- 0.88 Ga");
    compass.setScale(((float)0.88)); // Set the scale of the compass.

    Serial.println("Setting measurement mode to continous.");
    error = compass.setMeasurementMode(MEASUREMENT_CONTINUOUS); // Set the measurement mode to Continuous
}

void output();

void loop() {
    // Retrived the scaled values from the compass (scaled to the configured scale).
    MagnetometerScaled scaled = compass.readScaledAxis();

    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    float heading = atan2(scaled.YAxis, scaled.XAxis);

    Serial.println("WHERE AM IIIIIIII");
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

    output();
}

float getHeading() {
    float headingsSum = 0;
    for(int i=0; i<MAX_HEADINGS; i++)
        headingsSum += lastHeadings[i];

    return headingsSum/MAX_HEADINGS;
}

void output() {
	Serial.print("Heading: ");
	Serial.println(getHeading());
}
