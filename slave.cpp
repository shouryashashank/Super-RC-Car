#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08

// Motor pin mappings (IN1, IN2 pairs)
const int FL_IN1 = D5; const int FL_IN2 = D6;
const int RL_IN1 = D7; const int RL_IN2 = D8;
const int FR_IN1 = 1;  const int FR_IN2 = 3;  // TX and RX pins
const int RR_IN1 = D3; const int RR_IN2 = D4; // D4 is also the built-in LED

void setup() {
  // Initialize all motor pins as outputs
  pinMode(FL_IN1, OUTPUT); pinMode(FL_IN2, OUTPUT);
  pinMode(RL_IN1, OUTPUT); pinMode(RL_IN2, OUTPUT);
  pinMode(FR_IN1, OUTPUT); pinMode(FR_IN2, OUTPUT);
  pinMode(RR_IN1, OUTPUT); pinMode(RR_IN2, OUTPUT);

  // Stop all motors initially
  stopAllMotors();

  // Set high PWM frequency (16kHz) for silent, smoother low-speed operation
  analogWriteFreq(16000); 

  // Initialize I2C as Slave on pins D2 (SDA) and D1 (SCL)
  Wire.begin(D2, D1, I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // Keep loop empty; everything is handled via I2C interrupts
  delay(10);
}

// I2C Receive Interrupt Handler
void receiveEvent(int howMany) {
  // We expect exactly 2 bytes: [Motor ID] [Speed/Direction]
  if (Wire.available() >= 2) {
    byte motorId = Wire.read();
    int targetSpeed = (int8_t)Wire.read(); // Read as signed 8-bit integer (-128 to 127)

    setMotor(motorId, targetSpeed);
  }
}

// Function to drive individual motors
// speed: -100 to 100 (Negative = Reverse, Positive = Forward, 0 = Stop)
void setMotor(byte motor, int speed) {
  int pinIN1, pinIN2;

  // Assign physical pins based on Motor ID
  switch(motor) {
    case 1: pinIN1 = FL_IN1; pinIN2 = FL_IN2; break; // Front Left
    case 2: pinIN1 = RL_IN1; pinIN2 = RL_IN2; break; // Rear Left
    case 3: pinIN1 = FR_IN1; pinIN2 = FR_IN2; break; // Front Right
    case 4: pinIN1 = RR_IN1; pinIN2 = RR_IN2; break; // Rear Right
    default: return; // Invalid motor ID
  }

  if (speed == 0) {
    // Coast to stop
    digitalWrite(pinIN1, LOW);
    digitalWrite(pinIN2, LOW);
  } 
  else if (speed > 0) {
    // Forward: PWM on IN1, LOW on IN2
    int pwmValue = map(speed, 0, 100, 0, 1023); // Map 0-100% to ESP8266 0-1023 PWM range
    analogWrite(pinIN1, pwmValue);
    digitalWrite(pinIN2, LOW);
  } 
  else {
    // Reverse: LOW on IN1, PWM on IN2
    int pwmValue = map(abs(speed), 0, 100, 0, 1023);
    digitalWrite(pinIN1, LOW);
    analogWrite(pinIN2, pwmValue);
  }
}

void stopAllMotors() {
  digitalWrite(FL_IN1, LOW); digitalWrite(FL_IN2, LOW);
  digitalWrite(RL_IN1, LOW); digitalWrite(RL_IN2, LOW);
  digitalWrite(FR_IN1, LOW); digitalWrite(FR_IN2, LOW);
  digitalWrite(RR_IN1, LOW); digitalWrite(RR_IN2, LOW);
}