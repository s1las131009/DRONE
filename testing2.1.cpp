#include <Arduino.h>
#include <Servo.h>      // Include the Servo library

Servo myServo;          // Create a Servo object called myServo
int potPin = A0;

void setup() {
    Serial.begin(9600);
    myServo.attach(9);  // Tell the library our servo is on pin 9
    Serial.println("Potentiometer servo control ready");
}

void loop() {

    int rawValue = analogRead(potPin);              // Read pot: 0–1023
    int angle = map(rawValue, 0, 1023, 0, 180);     // Convert to 0–180°
   
    myServo.write(angle);                            // Tell servo to go to this angle
  
    Serial.print("Raw Value: ");
    Serial.print(rawValue);
    Serial.print(" Angle: ");
    Serial.print(angle);
    Serial.println("°");
    delay(15);                                       // Give it 15ms to reach each position
}