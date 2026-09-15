#include "motor_controller.h"

#define PWM_FREQ        20000
#define PWM_RES         8

#define CH_MOT_L_IN1    0
#define CH_MOT_L_IN2    1
#define CH_MOT_R_IN1    2
#define CH_MOT_R_IN2    3

MotorController::MotorController()
    : wanderTimer(0), wanderState(0) {}

bool MotorController::init() {
    ledcSetup(CH_MOT_L_IN1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_MOT_L_IN2, PWM_FREQ, PWM_RES);
    ledcSetup(CH_MOT_R_IN1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_MOT_R_IN2, PWM_FREQ, PWM_RES);

    ledcAttachPin(PIN_MOTOR_L_IN1, CH_MOT_L_IN1);
    ledcAttachPin(PIN_MOTOR_L_IN2, CH_MOT_L_IN2);
    ledcAttachPin(PIN_MOTOR_R_IN1, CH_MOT_R_IN1);
    ledcAttachPin(PIN_MOTOR_R_IN2, CH_MOT_R_IN2);

    stop();
    return true;
}

void MotorController::driveLeft(int pwm) {
    pwm = constrain(pwm, -255, 255);
    if (pwm > 0) {
        ledcWrite(CH_MOT_L_IN1, pwm);
        ledcWrite(CH_MOT_L_IN2, 0);
    } else if (pwm < 0) {
        ledcWrite(CH_MOT_L_IN1, 0);
        ledcWrite(CH_MOT_L_IN2, -pwm);
    } else {
        ledcWrite(CH_MOT_L_IN1, 0);
        ledcWrite(CH_MOT_L_IN2, 0);
    }
}

void MotorController::driveRight(int pwm) {
    pwm = constrain(pwm, -255, 255);
    if (pwm > 0) {
        ledcWrite(CH_MOT_R_IN1, pwm);
        ledcWrite(CH_MOT_R_IN2, 0);
    } else if (pwm < 0) {
        ledcWrite(CH_MOT_R_IN1, 0);
        ledcWrite(CH_MOT_R_IN2, -pwm);
    } else {
        ledcWrite(CH_MOT_R_IN1, 0);
        ledcWrite(CH_MOT_R_IN2, 0);
    }
}

void MotorController::setSpeed(int left, int right) {
    driveLeft(left);
    driveRight(right);
}

void MotorController::stop() {
    setSpeed(0, 0);
}

void MotorController::resetWanderTimer() {
    wanderTimer = millis() + 1000;
    wanderState = 0;
}

void MotorController::updateWander() {
    uint32_t now = millis();
    if (now < wanderTimer) return;

    wanderState = (wanderState + 1) % 4;
    switch (wanderState) {
        case 0: // Stop and observe
            stop();
            wanderTimer = now + random(1500, 3500);
            break;
        case 1: // Slow forward exploration
            setSpeed(110, 110);
            wanderTimer = now + random(1000, 2500);
            break;
        case 2: // Stop
            stop();
            wanderTimer = now + random(1000, 2000);
            break;
        case 3: // Pivot turn
            if (random(2) == 0) setSpeed(120, -120);
            else setSpeed(-120, 120);
            wanderTimer = now + random(400, 900);
            break;
    }
}

void MotorController::wiggle() {
    for (int i = 0; i < 3; i++) {
        setSpeed(140, -140);
        delay(90);
        setSpeed(-140, 140);
        delay(90);
    }
    stop();
}

void MotorController::spinJoy() {
    setSpeed(160, -160);
    delay(550);
    stop();
}

void MotorController::nudgeForward() {
    setSpeed(130, 130);
    delay(180);
    stop();
}

void MotorController::backupShyly() {
    setSpeed(-120, -120);
    delay(220);
    stop();
}
