// Motor & encoder drivers

const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;

volatile unsigned long encoderCount = 0;
volatile unsigned int completedOneRevolution = false;
const int interupt = 0;
const int speed = 255;

boolean anticlockwise = true;
int correction = 0;


void encoderISR(){
    encoderCount++;
}

void rotateAnticlockwise(){
    anticlockwise = true;
    digitalWrite(M1,HIGH);
    digitalWrite(M2,HIGH);

}

void rotateClockwise(){
    anticlockwise = false;
    digitalWrite(M1,LOW);
    digitalWrite(M2,LOW);
}

void changeDirection(){
    if(anticlockwise){
        rotateClockwise();
    }else{
        rotateAnticlockwise();
    }
}

void startRotate(){
    analogWrite(E1,speed);
}

void stopRotate(){
    analogWrite(E1,0);
}

void oneRotation(int fullSpin){
    startRotate();
    while(encoderCount < fullSpin){}
    stopRotate();
    delay(2000);
    //correction = encoderCount - 3360;
}

int numTicks(){
    int correction = encoderCount - 3360;
    return correction;
}

int correctTicks(int correction){
    encoderCount = 0;
    startRotate();
    while(encoderCount < correction){}
    stopRotate();
    delay(2000);
    //Serial.println(encoderCount);
    return encoderCount - correction;
}

void setup(){
    attachInterrupt(interupt,encoderISR,FALLING);
    Serial.begin(9600);
    rotateAnticlockwise();
}

void loop(){
    
    int i = 0;
    int fullSpin = 3360;
    int correction = 0;
    
    while(i < 50){
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        oneRotation(fullSpin);
        correction = numTicks();
        changeDirection();
        //Serial.print("Fullspin: ");
        //Serial.println(fullSpin);
        //Serial.print("Correction: ");
        //Serial.println(correction);
        
        correction = correctTicks(correction);
        //Serial.print("EncoderCount: ");
        //Serial.println(encoderCount);
        //correction = numTicks();
        fullSpin = correction + 3360;
        changeDirection();
        
        encoderCount = 0;
        i++;
        Serial.println(i);
    }   
}
