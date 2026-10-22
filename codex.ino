/*#include <IRremote.hpp> 

const int RECV_PIN = 4; 

void setup() { 
  Serial.begin(9600); 
  // Initialize the IR receiver
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); 
  Serial.println("IR Receiver Initialized. Press a button on your remote!"); 
} 

void loop() { 
  if (IrReceiver.decode()) { 
    Serial.print("Protocol: "); 
    Serial.println(IrReceiver.decodedIRData.protocol); 

    // Display the code in Hexadecimal format
    Serial.print("Hex Code: 0x"); 
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); 

    // Display the code as a regular Decimal number
    Serial.print("Decimal Number: "); 
    Serial.println(IrReceiver.decodedIRData.decodedRawData, DEC); 

    Serial.println("------------------------------"); 
    
    // Receive the next value
    IrReceiver.resume(); 
  } 
}*/

/*#include <IRremote.h>

IRrecv IR(4);
int w_pin = 8;
int w2_pin = 6; // Pin 6 supports PWM (analogWrite) on Arduino Uno/Nano

// Variable to track the current brightness level of w2_pin (0 = off, 255 = max brightness)
int brightness = 0; 

void setup() {
  IR.enableIRIn();
  pinMode(w_pin, OUTPUT);
  pinMode(w2_pin, OUTPUT); // Handled as PWM output automatically by analogWrite
  Serial.begin(9600);
}

void loop() {
  if (IR.decode()) {
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    
    // --- Control for w_pin (ON/OFF) ---
    if (IR.decodedIRData.decodedRawData == 0xBA45FF00) {
      digitalWrite(w_pin, HIGH);
    }
    if (IR.decodedIRData.decodedRawData == 0xB946FF00) {
      digitalWrite(w_pin, LOW);
    }

    // --- Regulate Brightness for w2_pin ---
    // Button to Brighten / Turn ON Max
    if (IR.decodedIRData.decodedRawData == 0xB847FF00) {
      brightness += 51; // Increase brightness by ~20% per press (51 * 5 = 255)
      if (brightness > 255) {
        brightness = 255; // Keep it capped at maximum brightness
      }
      analogWrite(w2_pin, brightness);
      Serial.print("w2 Brightness: ");
      Serial.println(brightness);
    }
    
    // Button to Dim / Turn OFF
    if (IR.decodedIRData.decodedRawData == 0xBB44FF00) {
      brightness -= 51; // Decrease brightness by ~20% per press
      if (brightness < 0) {
        brightness = 0; // Keep it capped at completely off
      }
      analogWrite(w2_pin, brightness);
      Serial.print("w2 Brightness: ");
      Serial.println(brightness);
    }

    delay(300); // Reduced delay slightly for more responsive brightness stepping
    IR.resume();
  }
}
*/
#include <IRremote.h>

IRrecv IR(2);

// Existing LED pins
int w_pin = 8;
int w2_pin = 6; 
int brightness = 0; 

// --- New DC Motor Pins ---
const int EN1 = 5;  // PWM pin to regulate motor speed (Must connect to L298N ENA)
const int IN1 = 9;  // Direction pin 1
const int IN2 = 10; // Direction pin 2

// Variable to track motor speed (150 = minimum speed, 255 = maximum speed)
int motorSpeed = 160; 

void setup() {
  IR.enableIRIn();
  
  // LED Setup
  pinMode(w_pin, OUTPUT);
  pinMode(w2_pin, OUTPUT);
  
  // Motor Pins Setup
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Set default direction (Forward)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  Serial.begin(9600);
}

void loop() {
  if (IR.decode()) {
    Serial.println(IR.decodedIRData.decodedRawData, HEX);
    
    // --- Control for w_pin (ON/OFF) ---
    if (IR.decodedIRData.decodedRawData == 0xBA45FF00) {
      digitalWrite(w_pin, HIGH);
    }
    if (IR.decodedIRData.decodedRawData == 0xB946FF00) {
      digitalWrite(w_pin, LOW);
    }

    // --- Regulate Brightness for w2_pin ---
    if (IR.decodedIRData.decodedRawData == 0xB847FF00) {
      brightness = min(brightness + 31, 255);
      analogWrite(w2_pin, brightness);
    }
    if (IR.decodedIRData.decodedRawData == 0xBB44FF00) {
      brightness = max(brightness - 31, 0);
      analogWrite(w2_pin, brightness);
    }

    // --- Regulate DC Motor Speed ---
    // Button 0xBF40FF00: Increase Speed / Switch On
    if (IR.decodedIRData.decodedRawData == 0xBF40FF00) {
      // If motor is currently off, ensure direction pins are active
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      
      motorSpeed += 31; // Increase speed by ~20%
      if (motorSpeed > 255) motorSpeed = 255;
      
      analogWrite(EN1, motorSpeed);
      Serial.print("Motor Speed: ");
      Serial.println(motorSpeed);
    }
    
    // Button 0xBC43FF00: Decrease Speed / Switch Off
    if (IR.decodedIRData.decodedRawData == 0xBC43FF00) {
      motorSpeed -= 31; // Decrease speed by ~20%
      if (motorSpeed <= 100) {
        motorSpeed = 100;
        // Fully stop the motor by cutting direction pins
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
      }
      
      analogWrite(EN1, motorSpeed);
      Serial.print("Motor Speed: ");
      Serial.println(motorSpeed);
    }

    delay(300); 
    IR.resume();
  }
}

