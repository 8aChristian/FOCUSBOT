#include "motor_controller.h"

// 20kHz PWM frequency (eliminates audible motor whine)
#define MOTOR_PWM_FREQ       20000
#define MOTOR_PWM_RES        8 // 8-bit resolution (0-255)

#define CH_MOT_L1            0
#define CH_MOT_L2            1
#define CH_MOT_R1            2
#define CH_MOT_R2            3

MotorController::MotorController() 
    : curLeft(0), curRight(0), targetLeft(0), targetRight(0),
      nextActionTime(0), wanderState(0) {}

bool MotorController::init() {
    ledcSetup(CH_MOT_L1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(CH_MOT_L2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(CH_MOT_R1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(CH_MOT_R2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

    ledcAttachPin(PIN_MOTOR_L_IN1, CH_MOT_L1);
    ledcAttachPin(PIN_MOTOR_L_IN2, CH_MOT_L2);
    ledcAttachPin(PIN_MOTOR_R_IN1, CH_MOT_R1);
    ledcAttachPin(PIN_MOTOR_R_IN2, CH_MOT_R2);

    stop();
    return true;
}

void MotorController::applyPwm(uint8_t ch1, uint8_t ch2, int speed) {
    speed = constrain(speed, -255, 255);
    if (speed > 0) {
        ledcWrite(ch1, speed);
        ledcWrite(ch2, 0);
    } else if (speed < 0) {
        ledcWrite(ch1, 0);
        ledcWrite(ch2, -speed);
    } else {
        // Fast brake / stop
        ledcWrite(ch1, 0);
        ledcWrite(ch2, 0);
    }
}

void MotorController::setSpeeds(int leftSpeed, int rightSpeed) {
    targetLeft = leftSpeed;
    targetRight = rightSpeed;
    
    // Smooth ramp
    curLeft = targetLeft;
    curRight = targetRight;
    
    applyPwm(CH_MOT_L1, CH_MOT_L2, curLeft);
    applyPwm(CH_MOT_R1, CH_MOT_R2, curRight);
}

void MotorController::stop() {
    setSpeeds(0, 0);
}

void MotorController::resetWanderTimer() {
    nextActionTime = millis() + 1000;
    wanderState = 0;
    stop();
}

void MotorController::updateWander() {
    uint32_t now = millis();
    if (now < nextActionTime) return;

    // State machine for organic desk wandering
    switch (wanderState) {
        case 0: // Stop and look around
            stop();
            wanderState = 1;
            nextActionTime = now + random(1500, 3500);
            break;

        case 1: // Gentle roll forward
            setSpeeds(120, 120);
            wanderState = 2;
            nextActionTime = now + random(800, 1800);
            break;

        case 2: // Brief pause
            stop();
            wanderState = 3;
            nextActionTime = now + random(500, 1200);
            break;

        case 3: // Pivot turn (left or right randomly)
            if (random(2) == 0) {
                setSpeeds(-110, 110); // Pivot left
            } else {
                setSpeeds(110, -110); // Pivot right
            }
            wanderState = 0;
            nextActionTime = now + random(400, 900);
            break;
    }
}
