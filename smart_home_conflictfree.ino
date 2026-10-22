// MANDATORY: Must define timer before including IRremote header
#define IR_USE_TIMER2 

#include <IRremote.hpp>
#include <Servo.h>

// ==========================================
// PIN DEFINITIONS (Conflict-Free Assignment)
// ==========================================
const int IR_RECEIVE_PIN = 2;

// Servos
const int SERVO1_PIN = 7;
const int SERVO2_PIN = 8;

// LEDs
const int W_PIN  = 11; // Standard Digital ON/OFF (Safe on Pin 11)
const int W2_PIN = 5;  // Hardware PWM Brightness (Timer0 - Safe)

// L293D DC Motor (Digital ON/OFF Control via Analog Pins)
const int EN1_PIN = A2; // Motor Enable
const int IN1_PIN = A3; // Motor Direction 1
const int IN2_PIN = A4; // Motor Direction 2

// Ultrasonic Sensor & Alarm (Analog Pins A0 & A1)
const int TRIG_PIN   = A0;
const int ECHO_PIN   = A1;
const int BUZZER_PIN = 12;

// ==========================================
// GLOBAL OBJECTS & STATE VARIABLES
// ==========================================
Servo servo1;
Servo servo2;

int brightness = 0;
bool isMotorOn = false;

// Non-blocking Sonar Timer Variables
unsigned long lastSonarScanTime = 0;
const unsigned long SONAR_INTERVAL = 60; // Scan distance every 60ms

// Ultrasonic Alarm Variables
const int DISTANCE_THRESHOLD = 8;         // Distance threshold in cm
const unsigned long RING_DURATION = 2000; // Alarm ring time in ms
unsigned long alarmStartTime = 0;
bool isAlarmActive = false;
bool hasTriggered = false;

// IR Remote Hex Commands
const unsigned long BTN_W1_ON  = 0xBA45FF00;
const unsigned long BTN_W1_OFF = 0xB946FF00;
const unsigned long BTN_W2_UP  = 0xB847FF00;
const unsigned long BTN_W2_DN  = 0xBB44FF00;
const unsigned long BTN_MOT_UP = 0xBF40FF00; // Motor ON
const unsigned long BTN_MOT_DN = 0xBC43FF00; // Motor OFF
const unsigned long BTN_7      = 0xF807FF00; // Servo 1 -> 100°
const unsigned long BTN_8      = 0xEA15FF00; // Servo 1 -> 0°
const unsigned long BTN_9      = 0xF609FF00; // Servo 2 -> 100°
const unsigned long BTN_0      = 0xE619FF00; // Servo 2 -> 0°

// ==========================================
// HELPER FUNCTIONS
// ==========================================
void setMotorState(bool turnOn) {
  if (turnOn) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(EN1_PIN, HIGH); // Full 5V drive ON
    isMotorOn = true;
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(EN1_PIN, LOW);  // Drive OFF
    isMotorOn = false;
  }
}

// Non-blocking Sonar Scanner
void processUltrasonic() {
  if (millis() - lastSonarScanTime >= SONAR_INTERVAL) {
    lastSonarScanTime = millis();

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Timeout constrained to 10,000us (~1.7m max range) to avoid CPU blocking
    long duration = pulseIn(ECHO_PIN, HIGH, 10000); 
    int distance = duration * 0.034 / 2;

    if (distance > 0 && distance <= DISTANCE_THRESHOLD) {
      if (!hasTriggered) {
        isAlarmActive = true;
        hasTriggered = true;
        alarmStartTime = millis();
        digitalWrite(BUZZER_PIN, HIGH);
      }
    } else {
      hasTriggered = false;
    }
  }

  // Handle active alarm shutoff
  if (isAlarmActive && (millis() - alarmStartTime >= RING_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
    isAlarmActive = false;
  }
}

// ==========================================
// MAIN SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(9600);

  // Configure Pin Modes
  pinMode(W_PIN, OUTPUT);
  pinMode(W2_PIN, OUTPUT);
  pinMode(EN1_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Clear Output States
  digitalWrite(W_PIN, LOW);
  analogWrite(W2_PIN, 0);
  setMotorState(false);

  // Attach Servos and Initialize Positions
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Start IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("System Hardware Fully Initialized.");
}

void loop() {
  // 1. Process IR Remote Input
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code != 0) {
      Serial.print("IR Code Received: 0x");
      Serial.println(code, HEX);

      // Digital LED
      if (code == BTN_W1_ON) {
        digitalWrite(W_PIN, HIGH);
      } 
      else if (code == BTN_W1_OFF) {
        digitalWrite(W_PIN, LOW);
      }

      // Dimmable LED (PWM)
      else if (code == BTN_W2_UP) {
        brightness = min(brightness + 31, 255);
        analogWrite(W2_PIN, brightness);
      } 
      else if (code == BTN_W2_DN) {
        brightness = max(brightness - 31, 0);
        analogWrite(W2_PIN, brightness);
      }

      // Motor Control (ON/OFF)
      else if (code == BTN_MOT_UP) {
        setMotorState(true);
        Serial.println("DC Motor: ON");
      } 
      else if (code == BTN_MOT_DN) {
        setMotorState(false);
        Serial.println("DC Motor: OFF");
      }

      // Servo Angles
      else if (code == BTN_7) {
        servo1.write(100);
      } 
      else if (code == BTN_8) {
        servo1.write(0);
      } 
      else if (code == BTN_9) {
        servo2.write(100);
      } 
      else if (code == BTN_0) {
        servo2.write(0);
      }
    }

    IrReceiver.resume(); // Receive next frame
  }

  // 2. Non-blocking Sonar Processing
  processUltrasonic();
}