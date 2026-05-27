#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    
    Serial.println("Initialising sensor...");
    mpu.initialize();
    
    // Skip testConnection() entirely — I2C scanner already confirmed
    // the sensor is present at 0x68, WHO_AM_I just returns wrong ID
    // for 9250/9255 variants when using the MPU6050 library
    
    Serial.println("Sensor initialised — starting readings");
    Serial.println("Lay flat and still for 3 seconds...");
    delay(3000);
    Serial.println("Starting readings...");
    Serial.println("------------------------------------------");
}

void loop() {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // Convert to real units
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    
    float gyroX = gx / 131.0;
    float gyroY = gy / 131.0;
    float gyroZ = gz / 131.0;
    
    // Print accelerometer data
    Serial.print("Accel (g) | X: ");
    Serial.print(accelX, 2);
    Serial.print("  Y: ");
    Serial.print(accelY, 2);
    Serial.print("  Z: ");
    Serial.print(accelZ, 2);
    
    Serial.print("   ||   ");
    
    // Print gyroscope data
    Serial.print("Gyro (°/s) | X: ");
    Serial.print(gyroX, 2);
    Serial.print("  Y: ");
    Serial.print(gyroY, 2);
    Serial.print("  Z: ");
    Serial.println(gyroZ, 2);
    
    delay(100);
}