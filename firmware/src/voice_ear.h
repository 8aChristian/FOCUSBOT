#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "pinout.h"
#include "robot_types.h"

class VoiceEar {
public:
    VoiceEar();
    bool init();
    VoiceEvent listen();

private:
    bool initialized;
    uint32_t lastEnergyTime;
    uint32_t speechStartTime;
    int consecutiveSpeechFrames;
    int tapCooldown;
};
