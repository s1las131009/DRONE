#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    
    Serial.println("Initialising sensor...");
    mpu.initialize();
 
    Serial.println("Sensor initialised successfully");
    Serial.println("Lay the board flat and still for 3 seconds...");
    delay(3000);
    Serial.println("Reading angles — tilt the board to see them change");
    Serial.println("----------------------------------------------------");
}

void loop() {
    // Step 1 — read raw values from sensor
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // Step 2 — convert raw values to g-force units
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    
    // Step 3 — calculate tilt angles using trigonometry
    // atan2 returns radians, so multiply by (180/PI) to get degrees
    float rollAngle  = atan2(accelY, accelZ) * (180.0 / PI);
    float pitchAngle = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * (180.0 / PI);
    
    // Step 4 — print angles to Serial Monitor
    Serial.print("Roll: ");
    Serial.print(rollAngle, 1);    
    Serial.print("°");
    Serial.print("   |   ");
    Serial.print("Pitch: ");
    Serial.print(pitchAngle, 1);
    Serial.println("°");
    
    delay(50);  
}