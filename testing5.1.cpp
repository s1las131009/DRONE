#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <math.h>

MPU6050 mpu;
Servo balanceServo;

// --- PID tuning constants ---
const float Kp = 0.85;
const float Ki = 0.05;
const float Kd = 0.5;

// --- Direction: change to -1.0 if servo corrects the wrong way ---
const float direction = 1.0;

// --- Deadband ---
const float deadband = 0.0;

// --- Setpoint ---
const float setpoint = 0.0;

// --- Complementary filter ---
const float alpha = 0.90;

// --- D term filter ---
const float dFilterAlpha = 0.85;

// --- Pitch smoothing filter ---
const float pitchSmoothAlpha = 0.7;

// --- State variables ---
float pitchAngle         = 0.0;
float smoothedPitch      = 0.0;
float integral           = 0.0;
float previousError      = 0.0;
float filteredDerivative = 0.0;

// --- Gyroscope calibration offset ---
float gyroYOffset = 0.0;

// --- Timing ---
unsigned long previousTime = 0;

// --- Servo limits ---
const int servoCentre = 90;
const int servoMin    = 20;
const int servoMax    = 175;

void setup() {
    Serial.begin(9600);
    Wire.begin();

    Serial.println("Initialising sensor...");
    mpu.initialize();
    // No testConnection() — WHO_AM_I incompatibility with 9250/9255 variants

    balanceServo.attach(9);
    balanceServo.write(servoCentre);

    // ---------------------------------------------------
    // GYROSCOPE CALIBRATION
    // Keep sensor completely still during this step
    // ---------------------------------------------------
    Serial.println("Calibrating gyroscope — keep sensor COMPLETELY STILL...");

    long gyroSum = 0;
    for (int i = 0; i < 2000; i++) {
        int16_t ax, ay, az, gx, gy, gz;
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        gyroSum += gy;
        delay(1);
    }

    gyroYOffset = gyroSum / 2000.0;

    Serial.print("Gyro Y offset found: ");
    Serial.println(gyroYOffset);
    Serial.println("Calibration complete — lay platform flat for 3 seconds...");
    delay(3000);

    previousTime = millis();

    Serial.println("=== Self-Levelling Platform Running ===");
    Serial.println("Raw | Smooth | Error | P | D | Output | Servo");
    Serial.println("------------------------------------------------");
}

void loop() {
    // -------------------------------------------------------
    // STEP 1 — Calculate dt
    // -------------------------------------------------------
    unsigned long currentTime = millis();
    float dt = (currentTime - previousTime) / 1000.0;
    previousTime = currentTime;

    if (dt <= 0) return;

    // -------------------------------------------------------
    // STEP 2 — Read raw sensor values
    // -------------------------------------------------------
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;

    // Subtract calibration offset to remove gyro drift bias
    float gyroY = (gy - gyroYOffset) / 131.0;

    // -------------------------------------------------------
    // STEP 3 — Complementary filter (pitch axis)
    // -------------------------------------------------------
    float accelPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ))
                       * (180.0 / PI);
    pitchAngle = alpha * (pitchAngle + gyroY * dt)
               + (1.0 - alpha) * accelPitch;

    // Smooth the pitch to filter out vibrations from servo movement
    smoothedPitch = pitchSmoothAlpha * smoothedPitch
                  + (1.0 - pitchSmoothAlpha) * pitchAngle;

    // -------------------------------------------------------
    // STEP 4 — Calculate error using smoothed pitch
    // -------------------------------------------------------
    float error = setpoint - smoothedPitch;

    // -------------------------------------------------------
    // STEP 5 — Deadband
    // -------------------------------------------------------
    if (abs(error) < deadband) {
        error = 0.0;
    }

    // -------------------------------------------------------
    // STEP 6 — P term
    // -------------------------------------------------------
    float P = Kp * error;

    // -------------------------------------------------------
    // STEP 7 — I term (off until P and D are stable)
    // -------------------------------------------------------
    integral += error * dt;
    integral = constrain(integral, -50, 50);
    float I = Ki * integral;

    // -------------------------------------------------------
    // STEP 8 — D term with low-pass filter
    // -------------------------------------------------------
    float rawDerivative = (error - previousError) / dt;
    filteredDerivative  = dFilterAlpha * filteredDerivative
                        + (1.0 - dFilterAlpha) * rawDerivative;
    float D = Kd * filteredDerivative;
    previousError = error;

    // -------------------------------------------------------
    // STEP 9 — Total PID output with direction control
    // -------------------------------------------------------
    float pidOutput = direction * (P + I + D);

    // -------------------------------------------------------
    // STEP 10 — Write to servo with independent min/max limits
    // -------------------------------------------------------
    int servoAngle = servoCentre + (int)pidOutput;
    servoAngle = constrain(servoAngle, servoMin, servoMax);
    balanceServo.write(servoAngle);

    // -------------------------------------------------------
    // STEP 11 — Print
    // -------------------------------------------------------
    Serial.print("Raw: ");
    Serial.print(pitchAngle, 1);
    Serial.print("°  Smooth: ");
    Serial.print(smoothedPitch, 1);
    Serial.print("°  |  Err: ");
    Serial.print(error, 2);
    Serial.print("  |  P: ");
    Serial.print(P, 2);
    Serial.print("  D: ");
    Serial.print(D, 2);
    Serial.print("  |  Out: ");
    Serial.print(pidOutput, 2);
    Serial.print("  |  Servo: ");
    Serial.print(servoAngle);
    Serial.println("°");

    delay(20);
}