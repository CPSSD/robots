#ifndef TapDetectionLib_H
#define TapDetectionLib_H

#include <ADXL345.h>

class TapDetectionLib {
    public:
        // Inits the accelerometer library.
        void init();

        // Returns true if a tap has been sensed.
        bool checkIfTapped();

    private:
        ADXL345 adxl;
};

#endif // TapDetectionLib_H