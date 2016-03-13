#ifndef VECTORMATH.H
#define VECTORMATH.H

#include "I2C_Wrapper.h"

class VectorMath{

    public:
        static bool setSign(int angle);
        static bool getSign();
        static double degreesToRadians(double angle);
        static double radiansToDegrees(double angle);
        static double computeSpeed(int angle);
    private:
        static bool positive;
        static bool FLBR_Wheels;
        static moveCommand currentMove;
};
#endif
