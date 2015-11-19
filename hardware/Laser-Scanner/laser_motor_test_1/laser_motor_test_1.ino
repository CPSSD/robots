//
// Test that encoder is working on the DC motor
//

// Motor driver pins：
const int E1 = 5;
const int M1 = 4;
const int E2 = 6;
const int M2 = 7;

// Encoder
const int interrupt = 0; // Pin 2
volatile unsigned int encoderCount = 0;
volatile unsigned int completedOneRevolution = false;

void encoderISR() {
  encoderCount = (encoderCount + 1) % 3360;
  if (encoderCount == 0) {
     completedOneRevolution = true;
  }
}

int readSpeed() {
  Serial.setTimeout(20000);
  Serial.println("Please input motor speed (150-255)");
  int speed = Serial.parseInt();
  if ((speed < 150) || (speed >255)) {
    speed = 255;
  }
  Serial.print("Speed set to ");
  Serial.print(speed);
  Serial.println();
  return speed;
}

void setup()
{
    attachInterrupt(interrupt,encoderISR,FALLING);

    digitalWrite(M1,HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 0);
    analogWrite(E2, 0);

    Serial.begin(115200);
    analogWrite(E1, readSpeed());
}

int revCount = 0;

void loop()
{
  if (completedOneRevolution) {
    completedOneRevolution = false;
    revCount++;
    Serial.println(revCount);
  }
}
