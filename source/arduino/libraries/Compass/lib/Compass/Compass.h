#ifndef Compass_H
#define Compass_H

#include <Arduino.h>
#include <HMC5883L.h>

class Compass {
	public:
        // Functions as a setup routine, inits the compass.
		Compass();
        // Unused, included for completeness.
		~Compass();
		
        // Returns the robots heading in degrees.
        float getHeading();
        // To be called once per loop. Updates the compass position.
        void updateHeading();
	

	private:
        // The compass object.
        HMC5883L compass;

        // Define the number of headings to average over to reduce error.
        static const int MAX_HEADINGS = 50;

        // List of the previous headings.
        float lastHeadings[MAX_HEADINGS];

        // Stores our current position in lastHeadings.
        int headingsIndex;
};

#endif
