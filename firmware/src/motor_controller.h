#pragma once
// =============================================================================
// motor_controller.h  —  FocusBot Dual-Motor Drive Controller
// =============================================================================
// Wraps DRV8833 dual H-bridge.  Provides:
//   • setSpeed(left, right)  — raw -255..+255 PWM values
//   • updateWander()         — autonomous wander state machine
//   • Cute choreography moves: wiggle, spinJoy, nudgeForward, backupShyly
// =============================================================================
#include <Arduino.h>
#include "pinout.h"

class MotorController {
public:
    MotorController();

    /// Configure PWM channels and pins.  Returns true on success.
    bool init();

    /// Immediately coast both motors to stop.
    void stop();

    /// Set left and right motor speeds independently.
    /// @param left   -255 (full reverse) .. +255 (full forward)
    /// @param right  -255 (full reverse) .. +255 (full forward)
    void setSpeed(int left, int right);

    /// Autonomous wander: call every loop in explore mode.
    void updateWander();

    /// Reset wander timing (call when switching into explore mode).
    void resetWanderTimer();

    // ------------------------------------------------------------------
    // Cute choreography  (blocking, short duration)
    // ------------------------------------------------------------------

    /// Quick left-right shimmy — expresses excitement / greeting.
    void wiggle();

    /// One full happy spin in place.
    void spinJoy();

    /// Short confident forward nudge.
    void nudgeForward();

    /// Small reverse — shy reaction to surprise / loud noise.
    void backupShyly();

private:
    uint32_t wanderTimer;
    int      wanderState;

    // Internal helpers — drive one side using two PWM pins
    void driveLeft(int pwm);
    void driveRight(int pwm);
};
