#pragma once
// =============================================================================
// servo_neck.h  —  FocusBot SG90 Neck Servo Controller
// =============================================================================
// Smooth non-blocking servo motion with a built-in choreography sequencer.
// Call update() every loop iteration to process motion steps.
//
// Cute motion catalogue:
//   nodYes()     — centre → left → right → centre (rapid agreement)
//   tiltCute()   — subtle single-side tilt then back
//   shakeNo()    — rapid left-right-left-right refusal
//   peekAround() — slow creep to one side, pause, then snap back
// =============================================================================
#include <Arduino.h>
#include <ESP32Servo.h>
#include "pinout.h"

class ServoNeck {
public:
    ServoNeck();

    /// Attach servo, set initial centre position.  Returns true on success.
    bool init();

    /// Must be called every main-loop iteration for smooth motion.
    void update();

    /// Set a new target angle with optional speed (degrees per millisecond).
    void setAngle(float deg, float speed = 2.0f);

    // ------------------------------------------------------------------
    // Preset positions
    // ------------------------------------------------------------------
    void lookCenter();
    void lookLeft();
    void lookRight();

    /// Autonomous idle scan — call repeatedly in explore mode.
    void scanCuriosity();

    // ------------------------------------------------------------------
    // Cute choreography (non-blocking — enqueues a step sequence)
    // ------------------------------------------------------------------

    /// Nod agreement: centre → left → right → centre  (fast)
    void nodYes();

    /// Subtle tilt one side then return.
    void tiltCute();

    /// Rapid left-right shake: refusal / confusion.
    void shakeNo();

    /// Slow creep to one side, pause, snap back.
    void peekAround();

private:
    Servo   servo;
    float   currentAngle;
    float   targetAngle;
    float   moveSpeed;       // deg / ms
    uint32_t lastUpdateMs;

    // Idle scan state
    uint32_t scanTimer;
    int      scanState;

    // ------------------------------------------------------------------
    // Internal choreography sequencer
    // ------------------------------------------------------------------
    struct Step {
        float    angle;    // target angle for this step
        uint32_t holdMs;   // how long to dwell at this angle before advancing
    };

    static const int MAX_STEPS = 8;
    Step     sequence[MAX_STEPS];
    int      seqLen;
    int      seqIdx;
    uint32_t stepTimer;
    bool     inSequence;

    void startSequence(const Step* steps, int len);
    void updateSequence(uint32_t now);
};
