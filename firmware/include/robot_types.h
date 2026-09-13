#pragma once
#include <Arduino.h>

// Operating Modes of FocusBot
enum RobotMode {
    MODE_BOOT_WAKEUP = 0,    // Initial hardware self-test & waking up animation
    MODE_DEFAULT_EXPLORE,   // Autonomous desk wander, cute expressions, voice listening
    MODE_FOCUS_GUARD,       // AI Vision monitoring: detects study vs procrastination
    MODE_LLM_COMPANION      // Paired with mobile app: conversational voice + LLM bridge
};

// Procedural Eye Expressions
enum EyeExpression {
    EXPR_NEUTRAL = 0,
    EXPR_BLINK,
    EXPR_HAPPY,
    EXPR_CURIOUS_LEFT,
    EXPR_CURIOUS_RIGHT,
    EXPR_FOCUS_SQUINT,
    EXPR_DROWSY,
    EXPR_HEART,
    EXPR_SURPRISED,
    EXPR_ANGRY_ALERT
};

// Sound Effects for MAX98357A I2S Synth
enum SoundEffect {
    SND_NONE = 0,
    SND_WAKEUP_CHIME,
    SND_HAPPY_CHIRP,
    SND_FOCUS_START,
    SND_PROCRASTINATION_ALERT,
    SND_CONFIRM_BOOP,
    SND_CURIOUS_TRILL,
    SND_SAD_WHINE,
    SND_PETTED_PURR
};

// Hardware Self-Test Status Flags
struct SystemHealth {
    bool display_ok;
    bool camera_ok;
    bool i2s_audio_ok;
    bool i2s_mic_ok;
    bool imu_ok;
    bool motors_ok;
    bool servo_ok;
    float battery_voltage;
    int battery_percentage;
};
