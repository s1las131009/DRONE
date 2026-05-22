#include <Arduino.h>

int ledPin = 9;

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    Serial.println("Type a number (0-255) and press Enter:");
}

void loop() {
    if (Serial.available() > 0) {       // If something has been typed and sent
        int input = Serial.parseInt();   // Read the number that was typed
        input = constrain(input, 0, 255); // Clamp it — won't let it go outside 0–255

        analogWrite(ledPin, input);      // Set LED brightness

        Serial.print("Brightness set to: ");
        Serial.println(input);
    }
}