#pragma once
#include <Arduino.h>

// =============================================================================
// FocusBot  —  robot_types.h
// Central type definitions shared across all firmware subsystems.
// =============================================================================

// ============================================================
// ROBOT OPERATING MODES
// ============================================================
enum RobotMode : uint8_t {
    MODE_BOOT_WAKEUP    = 0,
    MODE_DEFAULT_EXPLORE,
    MODE_FOCUS_GUARD,
    MODE_LLM_COMPANION,
    MODE_LOW_POWER,
    MODE_OTA_UPDATE
};

// ============================================================
// PROCEDURAL EYE EXPRESSIONS (Cozmo-style + FocusBot extras)
// ============================================================
enum EyeExpression : uint8_t {
    EXPR_NEUTRAL = 0,
    EXPR_BLINK_HIGH,
    EXPR_BLINK_LOW,
    EXPR_HAPPY,
    EXPR_GLEE,
    EXPR_SAD_DOWN,
    EXPR_SAD_UP,
    EXPR_WORRIED,
    EXPR_FOCUSED,
    EXPR_ANNOYED,
    EXPR_SURPRISED,
    EXPR_SKEPTIC,
    EXPR_FRUSTRATED,
    EXPR_UNIMPRESSED,
    EXPR_SLEEPY,
    EXPR_SUSPICIOUS,
    EXPR_SQUINT,
    EXPR_ANGRY,
    EXPR_FURIOUS,
    EXPR_SCARED,
    EXPR_AWE,
    EXPR_HEART,
    EXPR_THINKING,
    EXPR_WINK,
    EXPR_FOCUSED_TRACKING,
    EXPR_CELEBRATE,
    EXPR_ABSENT_ALERT,
    EXPR_LOW_BATTERY,
    EXPR_DROWSY,
    EXPR_CURIOUS_LEFT,
    EXPR_CURIOUS_RIGHT,

    // Aliases
    EXPR_FOCUS_SQUINT = EXPR_FOCUSED,
    EXPR_ANGRY_ALERT  = EXPR_FURIOUS,

    EXPR_COUNT
};

// ============================================================
// ILLUMINATION STATE
// ============================================================
enum IlluminationState : uint8_t {
    ILLUM_CYAN_IDLE = 0,   // 0x07FF — default idle cyan
    ILLUM_AMBER_FOCUS,     // 0xFD20 — warm amber during focus
    ILLUM_RED_ALERT,       // 0xF800 — alert red
    ILLUM_GREEN_WIN,       // 0x07E0 — celebration green
    ILLUM_PINK_HAPPY,      // 0xF81F — magenta / pink
    ILLUM_PULSE_THINKING,  // 0x07FF — cyan pulsing (0..1 brightness)
    ILLUM_DIM_BATTERY      // 0x6180 — dim blue-grey for low-battery
};

// ============================================================
// FOCUS SESSION STATE
// ============================================================
struct FocusState {
    int   score;
    int   continuousSec;
    int   totalFocusSec;
    int   milestoneMinutes;
    bool  procrastinating;
    bool  personPresent;
    float lookTargetX;
    float lookTargetY;
    float lookX;
    float lookY;
    bool  faceDetected;
};

// ============================================================
// SOUND EFFECTS
// ============================================================
enum SoundEffect : uint8_t {
    SND_NONE = 0,
    SND_WAKEUP_CHIME,
    SND_HAPPY_CHIRP,
    SND_FOCUS_START,
    SND_PROCRASTINATION_ALERT,
    SND_CONFIRM_BOOP,
    SND_CURIOUS_TRILL,
    SND_SAD_WHINE,
    SND_PETTED_PURR,
    SND_MILESTONE_FANFARE,
    SND_THINKING_HUM
};

// ============================================================
// VOICE EVENTS
// ============================================================
enum VoiceEvent : uint8_t {
    VOICE_NONE = 0,
    VOICE_TAP_HEAD,
    VOICE_COMMAND_FOCUS,
    VOICE_COMMAND_DEFAULT,
    VOICE_WAKEWORD_DETECTED,
    VOICE_LOUD_NOISE,
    VOICE_TALKING_ACTIVE
};

// ============================================================
// SYSTEM HEALTH
// ============================================================
struct SystemHealth {
    bool  display_ok;
    bool  camera_ok;
    bool  i2s_audio_ok;
    bool  i2s_mic_ok;
    bool  motors_ok;
    bool  servo_ok;
    float battery_voltage;
    int   battery_percentage;
};
