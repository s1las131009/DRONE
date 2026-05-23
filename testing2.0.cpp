#include <Arduino.h>
#include <Servo.h>      // Include the Servo library

Servo myServo;          // Create a Servo object called myServo

void setup() {
    Serial.begin(9600);
    myServo.attach(9);  // Tell the library our servo is on pin 9
    Serial.println("Servo sweep started");
}

void loop() {
    // Sweep from 0° to 180°
    for (int angle = 0; angle <= 180; angle++) {
        myServo.write(angle);   // Tell servo to go to this angle
        Serial.print("Angle: ");
        Serial.println(angle);
        delay(15);              // Give it 15ms to reach each position
    }

    // Sweep back from 180° to 0°
    for (int angle = 180; angle >= 0; angle--) {
        myServo.write(angle);
        Serial.print("Angle: ");
        Serial.println(angle);
        delay(15);
    }
}