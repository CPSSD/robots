#ifndef VECTORMATH.H
#define VECTORMATH.H

#include "I2C_Wrapper.h"

class VectorMath{

    public:
        static bool getSign();
        static double degreesToRadians(double angle);
        static double radiansToDegrees(double angle);
        static double computeSpeed(int angle);
        static double ticksPercentage(int angle, int distance);
    private:
        static int FL_Ticks(int angle);
        static int FL_Forward(int angle);
        static int FL_Backward(int angle);
        static int FR_Ticks(int angle);
        static int FR_Forward(int angle);
        static int FR_Backward(int angle);
        static bool positive;
        static bool FLBR_Wheels;
        static moveCommand currentMove;
};
#endif
