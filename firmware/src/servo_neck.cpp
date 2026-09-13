#include "servo_neck.h"

ServoNeck::ServoNeck() 
    : currentAngle(90.0f), targetAngle(90.0f), moveSpeed(2.0f), nextScanTime(0) {}

bool ServoNeck::init() {
    servo.setPeriodHertz(50); // Standard 50Hz servo
    servo.attach(PIN_SERVO_PWM, 500, 2500); // 500us to 2500us pulse
    lookCenter();
    return true;
}

void ServoNeck::setAngle(float angle, float speed) {
    targetAngle = constrain(angle, 35.0f, 145.0f);
    moveSpeed = speed;
}

void ServoNeck::lookCenter() {
    setAngle(90.0f, 3.0f);
}

void ServoNeck::update() {
    if (abs(currentAngle - targetAngle) > 0.5f) {
        if (currentAngle < targetAngle) {
            currentAngle += moveSpeed;
            if (currentAngle > targetAngle) currentAngle = targetAngle;
        } else {
            currentAngle -= moveSpeed;
            if (currentAngle < targetAngle) currentAngle = targetAngle;
        }
        servo.write((int)currentAngle);
    }
}

void ServoNeck::scanCuriosity() {
    uint32_t now = millis();
    if (now > nextScanTime) {
        // Pick new random curious head angle
        float newAngle = 90.0f + random(-40, 41);
        setAngle(newAngle, 1.5f);
        nextScanTime = now + random(2000, 5000);
    }
}
