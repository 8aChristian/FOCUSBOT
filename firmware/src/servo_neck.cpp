#include "servo_neck.h"

ServoNeck::ServoNeck()
    : currentAngle(90.0f), targetAngle(90.0f), moveSpeed(2.0f),
      lastUpdateMs(0), scanTimer(0), scanState(0),
      seqLen(0), seqIdx(0), stepTimer(0), inSequence(false) {}

bool ServoNeck::init() {
    servo.setPeriodHertz(50);
    servo.attach(PIN_SERVO_PWM, 500, 2500);
    servo.write(90);
    currentAngle = 90.0f;
    targetAngle = 90.0f;
    lastUpdateMs = millis();
    scanTimer = millis() + 2000;
    return true;
}

void ServoNeck::update() {
    uint32_t now = millis();
    uint32_t dt = (lastUpdateMs == 0) ? 20 : (now - lastUpdateMs);
    lastUpdateMs = now;

    if (inSequence) {
        updateSequence(now);
    }

    if (abs(currentAngle - targetAngle) > 0.5f) {
        float maxStep = moveSpeed * (float)dt;
        if (targetAngle > currentAngle) {
            currentAngle = min(targetAngle, currentAngle + maxStep);
        } else {
            currentAngle = max(targetAngle, currentAngle - maxStep);
        }
        servo.write((int)round(currentAngle));
    }
}

void ServoNeck::setAngle(float deg, float speed) {
    targetAngle = constrain(deg, 30.0f, 150.0f);
    moveSpeed = speed;
}

void ServoNeck::lookCenter() {
    inSequence = false;
    setAngle(90.0f, 2.5f);
}

void ServoNeck::lookLeft() {
    inSequence = false;
    setAngle(60.0f, 2.0f);
}

void ServoNeck::lookRight() {
    inSequence = false;
    setAngle(120.0f, 2.0f);
}

void ServoNeck::scanCuriosity() {
    if (inSequence) return;
    uint32_t now = millis();
    if (now > scanTimer) {
        scanState = (scanState + 1) % 4;
        switch (scanState) {
            case 0: setAngle(90.0f, 1.2f);  scanTimer = now + random(2000, 4000); break;
            case 1: setAngle(70.0f, 1.0f);  scanTimer = now + random(1500, 3000); break;
            case 2: setAngle(90.0f, 1.2f);  scanTimer = now + random(1000, 2500); break;
            case 3: setAngle(110.0f, 1.0f); scanTimer = now + random(1500, 3000); break;
        }
    }
}

void ServoNeck::startSequence(const Step* steps, int len) {
    seqLen = min(len, MAX_STEPS);
    for (int i = 0; i < seqLen; i++) {
        sequence[i] = steps[i];
    }
    seqIdx = 0;
    inSequence = true;
    stepTimer = millis() + sequence[0].holdMs;
    setAngle(sequence[0].angle, 3.5f);
}

void ServoNeck::updateSequence(uint32_t now) {
    if (!inSequence || seqLen == 0) return;
    if (now >= stepTimer) {
        seqIdx++;
        if (seqIdx < seqLen) {
            stepTimer = now + sequence[seqIdx].holdMs;
            setAngle(sequence[seqIdx].angle, 3.5f);
        } else {
            inSequence = false;
            seqLen = 0;
        }
    }
}

void ServoNeck::nodYes() {
    const Step steps[4] = {
        {80.0f, 120},
        {100.0f, 120},
        {85.0f, 120},
        {90.0f, 150}
    };
    startSequence(steps, 4);
}

void ServoNeck::tiltCute() {
    float tiltDir = (random(2) == 0) ? 75.0f : 105.0f;
    const Step steps[2] = {
        {tiltDir, 700},
        {90.0f, 300}
    };
    startSequence(steps, 2);
}

void ServoNeck::shakeNo() {
    const Step steps[5] = {
        {65.0f, 100},
        {115.0f, 100},
        {70.0f, 100},
        {110.0f, 100},
        {90.0f, 150}
    };
    startSequence(steps, 5);
}

void ServoNeck::peekAround() {
    float dir = (random(2) == 0) ? 55.0f : 125.0f;
    const Step steps[3] = {
        {dir, 800},
        {dir, 600},
        {90.0f, 250}
    };
    startSequence(steps, 3);
}
