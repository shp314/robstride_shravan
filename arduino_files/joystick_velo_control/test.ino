#include "RobstrideControl.h"

// Initialize motors
Motor joint1(21);
int vel = 0;  // Initialize velocity variable
float pos = 0.0;  // Initialize position variable
bool positionMode = false;  // Track control mode
unsigned long lastStateCheck = 0;  // Last state check timestamp
const unsigned long STATE_CHECK_INTERVAL = 100;  // Check state every 100ms

void setup() {
    Serial.begin(115200);
    initializeCAN(); // Initialize CAN bus

    // Reset all joint positions
    // joint1.resetPosition();
    joint1.setVelocity(0);

    Serial.println("System Initialized");
    Serial.println("Commands:");
    Serial.println("v[speed] - Set velocity mode (e.g., v10)");
    Serial.println("p[position] - Set position mode (e.g., p3.14)");
    Serial.println("r - Set current position as home (zero) position");
}

void loop() {
    // Handle commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read command
        command.trim(); // Remove whitespace
        
        if (command.length() > 0) {
            char mode = command.charAt(0);
            String value = command.substring(1);
            
            if (mode == 'v') {  // Velocity mode
                positionMode = false;
                int newVel = value.toInt();
                if (vel != newVel) {
                    vel = newVel;
                    joint1.setVelocity(vel);
                    Serial.print("Set velocity to ");
                    Serial.println(vel);
                }
            }
            else if (mode == 'p') {  // Position mode
                positionMode = true;
                float newPos = value.toFloat();
                if (pos != newPos) {
                    pos = newPos;
                    joint1.setPosition(pos);
                    Serial.print("Set position to ");
                    Serial.println(pos, 3);
                }
            }
            else if (mode == 'r') {  // Set current position as home
                joint1.resetPosition();
                pos = 0.0;
                Serial.println("Current position set as home (zero) position");
            }
        }
    }

    // Periodically read and report actual state
    unsigned long currentTime = millis();
    if (currentTime - lastStateCheck >= STATE_CHECK_INTERVAL) {
        float currentPos = joint1.readParameter(MECH_POS);
        float currentVel = joint1.readParameter(MECH_VEL);
        
        Serial.print("Position: ");
        Serial.print(currentPos, 3);
        Serial.print(" rad, Velocity: ");
        Serial.print(currentVel, 3);
        Serial.println(" rad/s");
        
        lastStateCheck = currentTime;
    }
}
