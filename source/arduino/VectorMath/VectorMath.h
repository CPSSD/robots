#ifndef VECTORMATH.H
#define VECTORMATH.H

class VectorMath{

    public:
        static void setSign();
        static double degreesToRadians();
        static double radiansToDegrees();
        static double computeSpeed();
    private:
        static bool postive;
        static moveCommand I2C_Wrapper::currentMove;
}
#endif
