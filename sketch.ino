#include <Arduino.h>

// Define constants for pin connections
const int sensorPin = A0;   // Analog pin A0 for LM35 sensor
const int ledPin = 13;      // Digital pin 13 for onboard LED

// Define variables to track temperature and LED blink intervals
int temperature = 0;
int blinkInterval = 0;
volatile boolean ledState = false;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set up timer interrupt
  cli(); // Disable interrupts
  TCCR1A = 0; // Reset Timer1 control register A
  TCCR1B = 0; // Reset Timer1 control register B
  TCNT1 = 0; // Reset Timer1 counter value
  OCR1A = 15624; // Set compare match register for 1 second (16MHz / (256 * 1Hz) - 1)
  TCCR1B |= (1 << WGM12); // Turn on CTC mode
  TCCR1B |= (1 << CS12); // Set prescaler to 256 and start the timer
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt
  sei(); // Enable interrupts
}

void loop() {
  // Read temperature from LM35 sensor
  int rawValue = analogRead(sensorPin);
  float voltage = (rawValue / 1024.0) * 5.0;
  temperature = (voltage - 0.5) * 100;

  // Debugging output
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Check temperature and update LED blink interval
  if (temperature < 30) {
    blinkInterval = 500; // 500ms blink interval
  } else {
    blinkInterval = 1000; // 1000ms blink interval
  }
}

// Timer1 compare interrupt service routine
ISR(TIMER1_COMPA_vect) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
  }
}
