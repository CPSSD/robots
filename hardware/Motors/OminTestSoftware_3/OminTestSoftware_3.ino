//
// Check that motors & encoders are working, and that the motors have correct identification.
//

int D1 = 4;   // direction
int M1 = 5;   // PWM
int D2 = 7;
int M2 = 6;

int M1_Interrupt = 0;
int M2_Interrupt = 1;

volatile unsigned long M1_EncoderCount = 0;
volatile unsigned long M2_EncoderCount = 0;

void M1_EncoderISR() {
  M1_EncoderCount++;
}

void M2_EncoderISR() {
  M2_EncoderCount++;
}

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
    if (true) { //M1_EncoderCount == 0) {
      digitalWrite(D1,HIGH);
      digitalWrite(D2,HIGH);
      analogWrite(M1, 255);
      analogWrite(M2, 0);
      delay(1000);

      Serial.println();
      Serial.println();
      Serial.print("Encoder for M1: ");
      Serial.println(M1_EncoderCount);

      digitalWrite(D1,HIGH);
      digitalWrite(D2,HIGH);
      analogWrite(M1, 0);
      analogWrite(M2, 255);
      delay(1000);

      analogWrite(M2,0);

      Serial.print("Encoder for M2: ");
      Serial.println(M2_EncoderCount);
    }
    delay(2000);
 }
