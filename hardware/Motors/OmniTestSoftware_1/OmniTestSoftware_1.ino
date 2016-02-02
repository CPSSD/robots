//
// Attempt to make each encoder yield the same number of ticks over time and
// hopefully move in a straight line.
//
// We reset the encoder counts every 'delayTime' milliseconds, and reset the
// counters, but we keep a running total, i.e., we adjust speed based on the
// counts for the last period
//
// ** This does not seem to work and the total accounts diverge.
//

int D1 = 4;   // direction
int M1 = 5;   // PWM
int D2 = 7;                     
int M2 = 6;                       

int M1_Interrupt = 0;
int M2_Interrupt = 1;

volatile unsigned long M1_EncoderCount = 0;
volatile unsigned long M2_EncoderCount = 0;

int M1_TotalCount = 0;
int M2_TotalCount = 0;

void M1_EncoderISR() {
  M1_EncoderCount++;
}

void M2_EncoderISR() {
  M2_EncoderCount++;
}

int M1_Speed = 220; // Keep speed of M1 constant and adjust M2 as appropriate
int M2_Speed = 220;

// wheels

float circumference = 31.4159265; // cm
int ticksPerRevolution = 800;

int targetDistance = 200; //cm
int ticksRequired = (200/circumference)*800;  // 5093

// accuracy

int accuracy = 10;
int motorAdjustment = 10;
int delayTime = 200;

// Setup

void setup()
{
    attachInterrupt(M1_Interrupt,M1_EncoderISR,FALLING);
    attachInterrupt(M2_Interrupt,M2_EncoderISR,FALLING);
    pinMode(D1, OUTPUT);  
    pinMode(D2, OUTPUT);
    Serial.begin(115200);
}
 

// Loop

void loop()
{
    if (M1_TotalCount < ticksRequired) {
      digitalWrite(D1,HIGH);
      digitalWrite(D2,HIGH);      
      analogWrite(M1, M1_Speed);
      analogWrite(M2, M2_Speed); 
      delay(delayTime);
      // We attempt to make the make the encoder counters yield the same value over time.
      // An alternative approach would be to accumulate M1 counts & reset counters?
      if (abs(M1_EncoderCount-M2_EncoderCount) > accuracy) {
        if (M1_EncoderCount < M2_EncoderCount) {
          // reduce speed of M2
          M2_Speed = max(M2_Speed - motorAdjustment,0);
        } else {
          // increase speed of M2
          M2_Speed = min(M2_Speed + motorAdjustment,255);
        }
      }

      Serial.println();
      Serial.println();
      Serial.print("Encoder for M1: ");
      Serial.println(M1_EncoderCount);
      Serial.print("Encoder for M2: ");
      Serial.println(M2_EncoderCount);
      Serial.print("M2 speed:");
      Serial.println(M2_Speed);

      M1_TotalCount += M1_EncoderCount;
      M2_TotalCount += M2_EncoderCount;
      M1_EncoderCount = 0;
      M2_EncoderCount = 0;

    } else {
      analogWrite(M1, 0);
      analogWrite(M2, 0);
      Serial.print("Total for M1: ");
      Serial.println(M1_TotalCount);
      Serial.print("Total for M2: ");
      Serial.println(M2_TotalCount);
      delay(10000);
    }

 }
