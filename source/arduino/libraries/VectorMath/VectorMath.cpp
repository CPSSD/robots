#include "VectorMath.h"
#include "WheelMotors.h"
#include "Arduino.h"
#include "I2C_Wrapper.h"
#include "math.h"

#define PI 3.14159265359

moveCommand VectorMath::currentMove;
bool VectorMath::FLBR_Wheels = true;
bool VectorMath::positive;

double VectorMath::ticksPercentage(int angle, int distance){
    if(FLBR_Wheels){
        Serial.print("Angle: ");
        Serial.println(FL_Ticks(angle));
        Serial.print("Distance: ");
        Serial.println(distance);
        return (distance / 90.00) * FL_Ticks(angle); // 90.00;
    }else{
        Serial.print("Here: ");
        Serial.println(FR_Ticks(angle));
        double temp = distance / 90.00;
        temp = temp * FR_Ticks(angle);
        return temp;
    }
}


int VectorMath::FL_Ticks(int angle){
    if(angle > 315 || angle < 135){
        return FL_Forward(angle);
    }else{
        return FL_Backward(angle);
    }
}

int VectorMath::FL_Forward(int angle){
    if(angle > 315){
        angle = angle - 315;
    }else if(angle < 45){
        angle = angle + 45;
    }else if(angle > 44 && angle < 135){
        angle = 135 - angle;
    }
    return angle;
}

int VectorMath::FL_Backward(int angle){
    if(angle > 134 && angle < 225){
        angle = angle - 135;
        //Serial.println("Made it");
    }else{
        angle = 315 - angle;
    }
    return angle;
}

int VectorMath::FR_Ticks(int angle){
    if(angle < 45 || angle > 225){
        return FR_Forward(angle);
    }else{
        return FR_Backward(angle);
    }
}

int VectorMath::FR_Forward(int angle){
    if(angle < 45){
        angle = abs(angle - 45);
    }else if(angle > 314){
        angle = 405 - angle;
    }else if(angle > 225 && angle < 315){
        angle = angle - 225;
    }
    return angle;
}

int VectorMath::FR_Backward(int angle){
    if(angle < 226 && angle > 135){
        return abs(angle - 225);
    }else{
        return angle - 45;
    }
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
