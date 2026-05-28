#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;
int ledPin = 9;  

void setup() {
    Serial.begin(9600);
    Wire.begin();
    pinMode(ledPin, OUTPUT);
    
    Serial.println("Initialising sensor...");
    mpu.initialize();
    
    Serial.println("Tilt Indicator Ready");
    Serial.println("Flat = LED off | Tilted = LED brightens | Vertical = LED full");
    delay(2000);
}

void loop() {
    // Step 1 — read raw sensor values
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // Step 2 — convert to g-force units
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    
    // Step 3 — calculate roll angle
    float rollAngle = atan2(accelY, accelZ) * (180.0 / PI);
    
    // Step 4 — take the absolute value (remove any negative sign)
    float absTilt = abs(rollAngle);

    // Step 5 — clamp and convert tilt angle to LED brightness
    // We cap at 90° — past vertical the LED is already fully on
    absTilt = constrain(absTilt, 0, 90);
    int brightness = map(absTilt, 0, 90, 0, 255);
    
    analogWrite(ledPin, brightness);
    
    // Step 7 — print everything to Serial Monitor
    Serial.print("Roll: ");
    Serial.print(rollAngle, 1);
    Serial.print("°");
    Serial.print("   |   Abs tilt: ");
    Serial.print(absTilt, 1);
    Serial.print("°");
    Serial.print("   |   Brightness: ");
    Serial.print(brightness);
    Serial.println("/255");
    
    delay(50);
}