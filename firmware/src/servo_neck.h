#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include "pinout.h"

class ServoNeck {
public:
    ServoNeck();
    bool init();
    void setAngle(float targetAngle, float speed = 2.0f);
    void update();
    void lookCenter();
    void scanCuriosity();

private:
    Servo servo;
    float currentAngle;
    float targetAngle;
    float moveSpeed;
    uint32_t nextScanTime;
};
