// Define Pin Numbers
const int TRIG_PIN = 9;
const int ECHO_PIN = 8;
const int BUZZER_PIN = 11;

// Distance threshold in centimeters
const int DISTANCE_THRESHOLD = 8; 

// Ring duration in milliseconds (3 seconds)
const unsigned long RING_DURATION = 2000; 

unsigned long alarmStartTime = 0;
bool isAlarmActive = false;
bool hasTriggered = false; // Reset state tracking flag

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Trigger Ultrasonic Sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read Echo pulse duration
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if an object is within the threshold distance
  if (distance > 0 && distance <= DISTANCE_THRESHOLD) {
    // Only start a new 3-second cycle if it hasn't triggered yet for this presence
    if (!hasTriggered) {
      isAlarmActive = true;
      hasTriggered = true;        // Prevents continuous re-triggering while present
      alarmStartTime = millis();  // Start 3-second clock
      digitalWrite(BUZZER_PIN, HIGH);
    }
  } else {
    // Object taken away: reset flag so next time it is brought back, it rings again
    hasTriggered = false; 
  }

  // Turn off buzzer once 3 seconds pass
  if (isAlarmActive && (millis() - alarmStartTime >= RING_DURATION)) {
    digitalWrite(BUZZER_PIN, LOW);
    isAlarmActive = false;
  }

  delay(50);
}