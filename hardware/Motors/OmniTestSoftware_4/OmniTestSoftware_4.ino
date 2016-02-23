//
// Motor speed calibration
//
// We fix M1 and attempt to calibrate M2.
//

int D1 = 4;   // direction
int M1 = 5;   // PWM
int D2 = 7;
int M2 = 6;

int M1_Interrupt = 0;
int M2_Interrupt = 1;

volatile unsigned long M1_EncoderCount = 0;
volatile unsigned long M2_EncoderCount = 0;

//int M1_TotalCount = 0;
//int M2_TotalCount = 0;

void M1_EncoderISR() {
  M1_EncoderCount++;
}

void M2_EncoderISR() {
  M2_EncoderCount++;
}

int M1_Speed = 200; // Keep speed of M1 constant and adjust M2 as appropriate
int M2_Speed = 200;

// Setup

void setup()
{
    attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
    attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    Serial.begin(115200);
}

// Some useful functions

int findEncoderCountForM1() {
  M1_EncoderCount = 0;
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  analogWrite(M1, M1_Speed);
  analogWrite(M2, 0);
  delay(5000);
  analogWrite(M1, 0);
  return M1_EncoderCount/5; 
}

int findEncoderCountForM2() {
  M2_EncoderCount = 0;
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  analogWrite(M1, 0);
  analogWrite(M2, M2_Speed);
  delay(5000);
  analogWrite(M2, 0);
  return M2_EncoderCount/5; 
}

int M1_total = 0;
int M2_total = 0;


// Loop

void loop()
{
    int M1_number = findEncoderCountForM1();
    Serial.print("M1 : "); Serial.print(M1_number); Serial.print(" : "); Serial.println(M1_Speed);
    int M2_number = findEncoderCountForM2();
    Serial.print("M2 : "); Serial.print(M2_number); Serial.print(" : "); Serial.println(M2_Speed);

    
    if (M1_number > M2_number+5 ) {
        M2_Speed += 5;
    }
    if (M1_number <  M2_number-5 ) {
        M2_Speed -= 5;
    }
   

    Serial.println("\n\n");









    delay(2000);
 }
