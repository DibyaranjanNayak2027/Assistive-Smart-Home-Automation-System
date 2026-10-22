/*#define IR_USE_TIMER2 // Uses Timer2 for IR to avoid clashing with Servo/Timer1 PWM

#include <IRremote.hpp>
#include <Servo.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
// IR Receiver
const int IR_RECEIVE_PIN = 2;

// Servos
const int SERVO1_PIN = 7;
const int SERVO2_PIN = 6;

// LEDs
const int W_PIN = 8;
const int W2_PIN = 11; // Moved to Pin 11 for Timer 2 safe PWM

// L293D DC Motor
const int EN1 = 5;  // PWM Speed Pin
const int IN1 = 3;  // Direction Pin 1
const int IN2 = 4;  // Direction Pin 2

// Ultrasonic Sensor & Buzzer
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 12;

// ==========================================
// GLOBAL VARIABLES & OBJECTS
// ==========================================
Servo servo1;
Servo servo2;

int brightness = 0;
int motorSpeed = 255;

// Ultrasonic Alarm Variables
const int DISTANCE_THRESHOLD = 5;     // Threshold distance in cm
const unsigned long RING_DURATION = 1000; // Alarm duration in ms (2 seconds)
unsigned long alarmStartTime = 0;
bool isAlarmActive = false;
bool hasTriggered = false;

// IR Remote Hex Code Mapping
const unsigned long BTN_W1_ON  = 0xBA45FF00; // Turn W LED ON
const unsigned long BTN_W1_OFF = 0xB946FF00; // Turn W LED OFF
const unsigned long BTN_W2_UP  = 0xB847FF00; // Increase Brightness
const unsigned long BTN_W2_DN  = 0xBB44FF00; // Decrease Brightness
const unsigned long BTN_MOT_UP = 0xBF40FF00; // Increase Motor Speed
const unsigned long BTN_MOT_DN = 0xBC43FF00; // Decrease Motor Speed / Stop
const unsigned long BTN_7      = 0xF807FF00; // Servo 1 -> 100°
const unsigned long BTN_8      = 0xEA15FF00; // Servo 1 -> 0°
const unsigned long BTN_9      = 0xF609FF00; // Servo 2 -> 100°
const unsigned long BTN_0      = 0xE619FF00; // Servo 2 -> 0°

void setup() {
  Serial.begin(9600);

  // Pin Modes
  pinMode(W_PIN, OUTPUT);
  pinMode(W2_PIN, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Default Motor State (Forward)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Attach Servos & Initialize Positions
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Initialize IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Combined System Initialized Successfully!");
}

void loop() {
  // ----------------------------------------------------
  // 1. IR REMOTE HANDLING
  // ----------------------------------------------------
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code != 0) {
      Serial.print("IR Code Received: 0x");
      Serial.println(code, HEX);

      // --- Digital LED Control ---
      if (code == BTN_W1_ON) {
        digitalWrite(W_PIN, HIGH);
      } 
      else if (code == BTN_W1_OFF) {
        digitalWrite(W_PIN, LOW);
      }

      // --- PWM LED Brightness Control ---
      else if (code == BTN_W2_UP) {
        brightness = min(brightness + 31, 255);
        analogWrite(W2_PIN, brightness);
      } 
      else if (code == BTN_W2_DN) {
        brightness = max(brightness - 31, 0);
        analogWrite(W2_PIN, brightness);
      }

      // --- DC Motor Speed Regulation ---
      else if (code == BTN_MOT_UP) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        motorSpeed = min(motorSpeed + 31, 255);
        analogWrite(EN1, motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      } 
      else if (code == BTN_MOT_DN) {
        motorSpeed -= 31;
        if (motorSpeed <= 100) {
          motorSpeed = 0;
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
        }
        analogWrite(EN1, motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      }

      // --- Servo Angle Controls ---
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

    delay(150); // Debounce delay
    IrReceiver.resume();
  }

  // ----------------------------------------------------
  // 2. ULTRASONIC SENSOR & ALARM HANDLING
  // ----------------------------------------------------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read Echo pulse duration (30ms timeout prevents loop freeze)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  int distance = duration * 0.034 / 2;

  // Trigger alarm if object enters distance threshold
  if (distance > 0 && distance <= DISTANCE_THRESHOLD) {
    if (!hasTriggered) {
      isAlarmActive = true;
      hasTriggered = true;
      alarmStartTime = millis();
      digitalWrite(BUZZER_PIN, HIGH);
    }
  } else {
    hasTriggered = false; // Reset trigger state when object leaves
  }

  // Shut off alarm after timer expires
  if (isAlarmActive && (millis() - alarmStartTime >= RING_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
    isAlarmActive = false;
  }
}*/
#define IR_USE_TIMER2 // IR remote uses Timer2 (Disables PWM on Pins 3 & 11)

#include <IRremote.hpp>
#include <Servo.h>

// ==========================================
// PIN DEFINITIONS (Re-allocated for Timer Safety)
// ==========================================
// IR Receiver
const int IR_RECEIVE_PIN = 2;

// Servos (Timer1 handles Servos)
const int SERVO1_PIN = 7;
const int SERVO2_PIN = 6;

// LEDs
const int W_PIN = 8;
const int W2_PIN = 5; // Moved to Pin 5 (Timer0 PWM - safe from IR/Servo interference)

// L293D DC Motor
const int EN1 = 3;  // Moved to Pin 3 (or Pin 10) for Speed Control
const int IN1 = 4;  // Direction Pin 1
const int IN2 = 13; // Direction Pin 2

// Ultrasonic Sensor & Buzzer
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 12;

// ==========================================
// GLOBAL VARIABLES & OBJECTS
// ==========================================
Servo servo1;
Servo servo2;

int brightness = 0;
int motorSpeed = 160;

// Ultrasonic Alarm Variables
const int DISTANCE_THRESHOLD = 8;     // Threshold distance in cm
const unsigned long RING_DURATION = 1000; // Alarm duration in ms (2 seconds)
unsigned long alarmStartTime = 0;
bool isAlarmActive = false;
bool hasTriggered = false;

// IR Remote Hex Code Mapping
const unsigned long BTN_W1_ON  = 0xBA45FF00; // Turn W LED ON
const unsigned long BTN_W1_OFF = 0xB946FF00; // Turn W LED OFF
const unsigned long BTN_W2_UP  = 0xB847FF00; // Increase Brightness
const unsigned long BTN_W2_DN  = 0xBB44FF00; // Decrease Brightness
const unsigned long BTN_MOT_UP = 0xBF40FF00; // Increase Motor Speed
const unsigned long BTN_MOT_DN = 0xBC43FF00; // Decrease Motor Speed / Stop
const unsigned long BTN_7      = 0xF807FF00; // Servo 1 -> 100°
const unsigned long BTN_8      = 0xEA15FF00; // Servo 1 -> 0°
const unsigned long BTN_9      = 0xF609FF00; // Servo 2 -> 100°
const unsigned long BTN_0      = 0xE619FF00; // Servo 2 -> 0°

// Helper function to handle motor startup boost
void setMotorSpeed(int targetSpeed) {
  if (targetSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    // Kick-start boost if motor was previously stopped
    if (motorSpeed == 0) {
      analogWrite(EN1, 255);
      delay(50); 
    }
    analogWrite(EN1, targetSpeed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(EN1, 0);
  }
}

void setup() {
  Serial.begin(9600);

  // Pin Modes
  pinMode(W_PIN, OUTPUT);
  pinMode(W2_PIN, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Attach Servos & Initialize Positions
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Initialize IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Combined System Initialized Successfully!");
}

void loop() {
  // ----------------------------------------------------
  // 1. IR REMOTE HANDLING
  // ----------------------------------------------------
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code != 0) {
      Serial.print("IR Code Received: 0x");
      Serial.println(code, HEX);

      // --- Digital LED Control ---
      if (code == BTN_W1_ON) {
        digitalWrite(W_PIN, HIGH);
      } 
      else if (code == BTN_W1_OFF) {
        digitalWrite(W_PIN, LOW);
      }

      // --- PWM LED Brightness Control ---
      else if (code == BTN_W2_UP) {
        brightness = min(brightness + 31, 255);
        analogWrite(W2_PIN, brightness);
      } 
      else if (code == BTN_W2_DN) {
        brightness = max(brightness - 31, 0);
        analogWrite(W2_PIN, brightness);
      }

      // --- DC Motor Speed Regulation ---
      else if (code == BTN_MOT_UP) {
        motorSpeed = min(motorSpeed + 31, 255);
        setMotorSpeed(motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      } 
      else if (code == BTN_MOT_DN) {
        motorSpeed -= 31;
        if (motorSpeed <= 100) {
          motorSpeed = 0;
        }
        setMotorSpeed(motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      }

      // --- Servo Angle Controls ---
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

    delay(150); // Debounce delay
    IrReceiver.resume();
  }

  // ----------------------------------------------------
  // 2. ULTRASONIC SENSOR & ALARM HANDLING
  // ----------------------------------------------------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
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

  if (isAlarmActive && (millis() - alarmStartTime >= RING_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
    isAlarmActive = false;
  }
}

/*#define IR_USE_TIMER2 // IR remote uses Timer2 (Disables PWM on Pins 3 & 11)

#include <IRremote.hpp>
#include <Servo.h>

// ==========================================
// PIN DEFINITIONS (Conflict-Free Mapping)
// ==========================================
// IR Receiver
const int IR_RECEIVE_PIN = 2;

// Servos (Timer1 handles Servos)
const int SERVO1_PIN = 7;
const int SERVO2_PIN = 8;

// LEDs
const int W_PIN  = 11; // Digital ON/OFF (Safe on Pin 11)
const int W2_PIN = 9;  // PWM Brightness (Timer0 PWM - Safe from IR conflict)

// L293D DC Motor
const int EN1 = 5;  // PWM Speed Control (Timer0 PWM - Safe from IR conflict)
const int IN1 = 4;  // Direction Pin 1
const int IN2 = 13; // Direction Pin 2

// Ultrasonic Sensor (Moved to Analog Pins A0 & A1) & Buzzer
const int TRIG_PIN   = A0; // Analog A0 used as Digital Output
const int ECHO_PIN   = A1; // Analog A1 used as Digital Input
const int BUZZER_PIN = 12;

// ==========================================
// GLOBAL VARIABLES & OBJECTS
// ==========================================
Servo servo1;
Servo servo2;

int brightness = 0;
int motorSpeed = 160;

// Ultrasonic Alarm Variables
const int DISTANCE_THRESHOLD = 8;         // Threshold distance in cm
const unsigned long RING_DURATION = 2000; // Alarm duration in ms
unsigned long alarmStartTime = 0;
bool isAlarmActive = false;
bool hasTriggered = false;

// IR Remote Hex Code Mapping
const unsigned long BTN_W1_ON  = 0xBA45FF00;
const unsigned long BTN_W1_OFF = 0xB946FF00;
const unsigned long BTN_W2_UP  = 0xB847FF00;
const unsigned long BTN_W2_DN  = 0xBB44FF00;
const unsigned long BTN_MOT_UP = 0xBF40FF00;
const unsigned long BTN_MOT_DN = 0xBC43FF00;
const unsigned long BTN_7      = 0xF807FF00;
const unsigned long BTN_8      = 0xEA15FF00;
const unsigned long BTN_9      = 0xF609FF00;
const unsigned long BTN_0      = 0xE619FF00;

// Helper function to handle motor startup boost
void setMotorSpeed(int targetSpeed) {
  if (targetSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    
    // Kick-start boost if motor was previously stopped
    if (motorSpeed == 0) {
      analogWrite(EN1, 255);
      delay(50); 
    }
    analogWrite(EN1, targetSpeed);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(EN1, 0);
  }
}

void setup() {
  Serial.begin(9600);

  // Pin Modes
  pinMode(W_PIN, OUTPUT);
  pinMode(W2_PIN, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Attach Servos & Initialize Positions
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Initialize IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Combined System Initialized Successfully!");
}

void loop() {
  // ----------------------------------------------------
  // 1. IR REMOTE HANDLING
  // ----------------------------------------------------
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code != 0) {
      Serial.print("IR Code Received: 0x");
      Serial.println(code, HEX);

      // --- Digital LED Control ---
      if (code == BTN_W1_ON) {
        digitalWrite(W_PIN, HIGH);
      } 
      else if (code == BTN_W1_OFF) {
        digitalWrite(W_PIN, LOW);
      }

      // --- PWM LED Brightness Control ---
      else if (code == BTN_W2_UP) {
        brightness = min(brightness + 31, 255);
        analogWrite(W2_PIN, brightness);
      } 
      else if (code == BTN_W2_DN) {
        brightness = max(brightness - 31, 0);
        analogWrite(W2_PIN, brightness);
      }

      // --- DC Motor Speed Regulation ---
      else if (code == BTN_MOT_UP) {
        motorSpeed = min(motorSpeed + 31, 255);
        setMotorSpeed(motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      } 
      else if (code == BTN_MOT_DN) {
        motorSpeed -= 31;
        if (motorSpeed <= 100) {
          motorSpeed = 0;
        }
        setMotorSpeed(motorSpeed);
        Serial.print("Motor Speed: ");
        Serial.println(motorSpeed);
      }

      // --- Servo Angle Controls ---
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

    delay(150); // Debounce delay
    IrReceiver.resume();
  }

  // ----------------------------------------------------
  // 2. ULTRASONIC SENSOR & ALARM HANDLING
  // ----------------------------------------------------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
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

  if (isAlarmActive && (millis() - alarmStartTime >= RING_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
    isAlarmActive = false;
  }
} */