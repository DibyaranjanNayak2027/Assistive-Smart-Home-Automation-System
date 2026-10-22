// Define timer BEFORE including IRremote
#define IR_USE_TIMER2

#include <IRremote.hpp>
#include <Servo.h>

// Define Pins
const int IR_RECEIVE_PIN = 2;
const int SERVO1_PIN = 3;
const int SERVO2_PIN = 6;

// Define Remote Hex Codes
const unsigned long BTN_7 = 0xF807FF00; // Servo 1 -> 100°
const unsigned long BTN_8 = 0xEA15FF00; // Servo 1 -> 0°
const unsigned long BTN_9 = 0xF609FF00; // Servo 2 -> 100°
const unsigned long BTN_0 = 0xE619FF00; // Servo 2 -> 0°

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(9600);
  
  // Attach standard Servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  
  // Natural position (0 degrees)
  servo1.write(0);
  servo2.write(0);

  // Initialize IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready (Running on Timer2)");
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData != 0) {
      unsigned long code = IrReceiver.decodedIRData.decodedRawData;

      if (code == BTN_7) {
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
    IrReceiver.resume();
  }
}