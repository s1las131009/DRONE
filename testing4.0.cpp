#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

// --- Filter tuning ---
const float alpha = 0.98;  // Trust factor: 0.98 = 98% gyro, 2% accel
                            

// --- Angle estimates (these get updated every loop) ---
float rollAngle  = 0.0;
float pitchAngle = 0.0;

// --- Timing variables ---
unsigned long previousTime = 0; 

void setup() {
    Serial.begin(9600);
    Wire.begin();
    
    Serial.println("Initialising sensor...");
    mpu.initialize();
    
    Serial.println("Sensor initialised");
    Serial.println("Lay the board flat and still for calibration...");
    delay(3000);  // Give user time to lay board flat before readings start
    
    // Initialise previousTime right before the loop starts
    // so the first dt calculation isn't huge
    previousTime = millis();
    
    Serial.println("Complementary filter running");
    Serial.println("-------------------------------");
}

void loop() {
    // -------------------------------------------------------
    // STEP 1 — Calculate dt (time since last loop)
    // -------------------------------------------------------
    unsigned long currentTime = millis();
    float dt = (currentTime - previousTime) / 1000.0;  // Convert ms to seconds
    previousTime = currentTime;  // Save current time for next loop
    
    // -------------------------------------------------------
    // STEP 2 — Read raw sensor data
    // -------------------------------------------------------
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // -------------------------------------------------------
    // STEP 3 — Convert to real units
    // -------------------------------------------------------
    // Accelerometer: raw → g-force
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    
    // Gyroscope: raw → degrees per second
    float gyroX = gx / 131.0;
    float gyroY = gy / 131.0;
    
    // -------------------------------------------------------
    // STEP 4 — Calculate accelerometer angle
    // -------------------------------------------------------
    float accelRoll  = atan2(accelY, accelZ) * (180.0 / PI);
    float accelPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * (180.0 / PI);
    
    // -------------------------------------------------------
    // STEP 5 — Apply complementary filter
    // Blend gyroscope integration with accelerometer correction
    // -------------------------------------------------------
    rollAngle  = alpha * (rollAngle  + gyroX * dt) + (1.0 - alpha) * accelRoll;
    pitchAngle = alpha * (pitchAngle + gyroY * dt) + (1.0 - alpha) * accelPitch;
    
    // -------------------------------------------------------
    // STEP 6 — Print results
    // -------------------------------------------------------
    Serial.print("Roll: ");
    Serial.print(rollAngle, 2);
    Serial.print("°");
    Serial.print("   |   Pitch: ");
    Serial.print(pitchAngle, 2);
    Serial.println("°");
}