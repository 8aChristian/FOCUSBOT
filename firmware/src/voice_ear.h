#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "pinout.h"
#include "robot_types.h"

enum VoiceEvent {
    VOICE_NONE = 0,
    VOICE_TAP_HEAD,         // Mechanical tap on headboard (petting)
    VOICE_WAKEWORD_DETECTED,// "Hey Focus" or double acoustic spike
    VOICE_COMMAND_DEFAULT,  // "Default Mode"
    VOICE_COMMAND_FOCUS,    // "Focus Mode"
    VOICE_TALKING_ACTIVE    // Continuous voice speech
};

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
