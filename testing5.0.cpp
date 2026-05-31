#include <Arduino.h>

// --- Pin definitions ---
const int ldrPin = A0;   // LDR on analogue pin A0
const int ledPin = 9;    // LED on PWM pin 9

// --- PID tuning constants ---
// These are starting values — you will need to tune them
const float Kp = 2.0;    // Proportional gain
const float Ki = 0.05;   // Integral gain
const float Kd = 0.5;    // Derivative gain

// --- Setpoint ---
// This is the light level you want the LDR to read (0–1023)
// Set this to roughly the reading when the LED is at medium brightness
const float setpoint = 500.0;

// --- PID state variables (must persist between loops) ---
float integral    = 0.0;
float previousError = 0.0;

// --- Timing ---
unsigned long previousTime = 0;

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    
    Serial.println("=== PID Light Controller ===");
    Serial.println("Cover/uncover LDR to disturb the system");
    Serial.println("Setpoint | Measured | Error | P | I | D | Output");
    Serial.println("------------------------------------------------------");
    
    previousTime = millis();
}

void loop() {
    // -------------------------------------------------------
    // STEP 1 — Calculate dt
    // -------------------------------------------------------
    unsigned long currentTime = millis();
    float dt = (currentTime - previousTime) / 1000.0;
    previousTime = currentTime;
    
    // Prevent dt being zero on first loop (would cause division by zero in D term)
    if (dt <= 0) return;
    
    // -------------------------------------------------------
    // STEP 2 — Read sensor (current value)
    // -------------------------------------------------------
    float measured = analogRead(ldrPin);   // Current light level: 0–1023
    
    // -------------------------------------------------------
    // STEP 3 — Calculate error
    // -------------------------------------------------------
    float error = setpoint - measured;
    
    // -------------------------------------------------------
    // STEP 4 — Calculate P term
    // -------------------------------------------------------
    float P = Kp * error;
    
    // -------------------------------------------------------
    // STEP 5 — Calculate I term
    // Accumulate error over time, with windup protection
    // -------------------------------------------------------
    integral += error * dt;
    
    // Windup protection — clamp integral to prevent it growing too large
    integral = constrain(integral, -500, 500);
    
    float I = Ki * integral;
    
    // -------------------------------------------------------
    // STEP 6 — Calculate D term
    // Rate of change of error
    // -------------------------------------------------------
    float derivative = (error - previousError) / dt;
    float D = Kd * derivative;
    
    previousError = error;   // Save for next loop
    
    // -------------------------------------------------------
    // STEP 7 — Calculate total output
    // -------------------------------------------------------
    float output = P + I + D;
    
    // Clamp output to valid PWM range
    output = constrain(output, 0, 255);
    
    // -------------------------------------------------------
    // STEP 8 — Apply output to LED
    // -------------------------------------------------------
    analogWrite(ledPin, (int)output);
    
    // -------------------------------------------------------
    // STEP 9 — Print everything to Serial Monitor
    // -------------------------------------------------------
    Serial.print("SP: ");
    Serial.print(setpoint, 0);
    Serial.print("  |  Meas: ");
    Serial.print(measured, 0);
    Serial.print("  |  Err: ");
    Serial.print(error, 1);
    Serial.print("  |  P: ");
    Serial.print(P, 1);
    Serial.print("  I: ");
    Serial.print(I, 1);
    Serial.print("  D: ");
    Serial.print(D, 1);
    Serial.print("  |  Out: ");
    Serial.println(output, 1);
  
    delay(50);  // Give the LED time to physically affect the LDR
}