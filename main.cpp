#include <Arduino.h>

int potPin = A0;  // Potentiometer connected to analogue pin A0
int ledPin = 3;   // LED connected to digital pin 3 (PWM capable)

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);  // Tell the Arduino pin 3 is an output
    Serial.println("=== Brightness Controller Started ===");
}

void loop() {
    int rawValue = analogRead(potPin);                    // Read pot: 0–1023
    int brightness = map(rawValue, 0, 1023, 0, 255);     // Convert to PWM range
    int percentage = map(rawValue, 0, 1023, 0, 100);     // Convert to percentage

    analogWrite(ledPin, brightness);  // Set the LED brightness

    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print("  |  PWM: ");
    Serial.print(brightness);
    Serial.print("  |  Power: ");
    Serial.print(percentage);
    Serial.println("%");

    delay(100);
}