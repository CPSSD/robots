#include "VectorMath.h"
#include "WheelMotors.h"
#include "I2C_Wrapper.h"
#include "math.h"

#define PI 3.14159265359

moveCommand I2C_Wrapper::currentMove = NULL;
bool positive = true;

// code so the wheels know which ones are which

void setSign(){
    if(currentMove.angle > 0 && currentMove.angle < 181){
        postive = true;
    }else{
        postive = false;
    }
}

double degreesToRadians(int angle){
    return 180/angle * PI;
}

double radiansToDegrees(double angle){
    return 180 * angle/PI;
}

double computeSpeed(bool sign,int angle){
    if(postive){
        return 255 * cos((PI/4) - degreesToRadians(angle));
    }else{
        return 255 * cos((PI/4) + degreesToRadians(angle));
    }
}
