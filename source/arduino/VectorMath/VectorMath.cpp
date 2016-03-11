#include "VectorMath.h"
#include "WheelMotors.h"
#include "Arduino.h"
#include "I2C_Wrapper.h"
#include "math.h"

#define PI 3.14159265359

moveCommand VectorMath::currentMove;
bool VectorMath::FLBR_Wheels = true;
bool VectorMath::positive;


//sets sign to add or subtract theta
bool VectorMath::setSign(int angle){
    //wheels Front Left - Back Right
    if(FLBR_Wheels){
        // Movement is to 1st/4th Quadrant
        if(angle > 0 && angle < 180){
            positive = true;
        //Movement to 2nd/3rd Quadrant"
        }else{
            positive = false;
        }
    //wheels Front Right - Back Left
    }else{
        //Movement is to 2nd/3rd Quadrant
        if(angle > 179 && angle < 360){
            positive = true;
        //Movement to 1st/4th Quadrant
        }else{
            positive = false;
        }
    }
    return positive;
}

bool VectorMath::getSign(){
  return positive;
}

double VectorMath::degreesToRadians(double angle){
    return (PI/180)* angle;
}

double VectorMath::radiansToDegrees(double angle){
    return (180/PI) * angle;
}

double VectorMath::computeSpeed(int angle){
    if(FLBR_Wheels){
        //return degreesToRadians(angle);
        return 255 * cos((PI/4) - degreesToRadians(angle));
    }else{
        return 255 * cos((PI/4) + degreesToRadians(angle));
    }
}
