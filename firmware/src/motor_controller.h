#pragma once
#include <Arduino.h>
#include "pinout.h"

class MotorController {
public:
    MotorController();
    bool init();
    void setSpeeds(int leftSpeed, int rightSpeed); // -255 to +255
    void stop();
    void updateWander();
    void resetWanderTimer();

private:
    int curLeft;
    int curRight;
    int targetLeft;
    int targetRight;
    uint32_t nextActionTime;
    int wanderState;
    
    void applyPwm(uint8_t pin1, uint8_t pin2, int speed);
};
