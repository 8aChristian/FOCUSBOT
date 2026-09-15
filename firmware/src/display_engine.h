#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "pinout.h"
#include "robot_types.h"

// =============================================================================
// DisplayEngine  —  FocusBot v12.0 Procedural Eye Renderer
//
// Drives a 320x240 ST7789 IPS display in landscape orientation.
// Renders two large, expressive square-rounded eyes in the Cozmo style:
//   - Cyan eyes on a pure-black background
//   - Per-expression draw routines for all Cozmo-sheet emotions
//   - Multi-layer glow halo + dark pupil + specular highlights
//   - Smooth auto-blink, saccade, and look-target interpolation
//   - ILLUM_PULSE_THINKING brightness bounce
//   - Focus-mode HUD bars (top + bottom strip)
//
// Screen coordinate convention (after setRotation(1)):
//   Origin top-left   X: 0..319 (left→right)   Y: 0..239 (top→bottom)
//
// Nominal eye centres (NEUTRAL):
//   Left  eye:  cx ≈ 103, cy = 120
//   Right eye:  cx ≈ 217, cy = 120
//   Formula:    lx = 320/2 − eyeW/2 − eyeSpacing/2
//               rx = 320/2 + eyeW/2 + eyeSpacing/2
// =============================================================================

class DisplayEngine {
public:
    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------
    DisplayEngine();

    /// Initialise TFT, set landscape rotation, clear to black.
    bool init();

    // -------------------------------------------------------------------------
    // Boot sequence
    // -------------------------------------------------------------------------
    /// BIOS-style boot diagnostic line with a cyan progress bar.
    /// progressPercent: 0..100
    void showBootDiagnostic(const char* subsystem, bool status, int progressPercent);

    /// Blocking wakeup animation: slit → wide-open eyes → happy flash.
    void playWakeupAnimation();

    // -------------------------------------------------------------------------
    // Expression & illumination control
    // -------------------------------------------------------------------------
    /// Immediately change the active eye expression.
    void setExpression(EyeExpression expr);

    /// Change the glow colour / animation mode used when drawing eyes.
    void setIllumination(IlluminationState illum);

    /// Set a normalised look target in [-1, +1].  Smoothly lerped each tick.
    void setLookTarget(float nx, float ny);

    // -------------------------------------------------------------------------
    // Animation pump  —  call every loop() iteration (50 Hz target)
    // -------------------------------------------------------------------------
    void updateAnimation();

    // -------------------------------------------------------------------------
    // HUD overlays
    // -------------------------------------------------------------------------
    /// Render the focus-mode top + bottom HUD bars for a given FocusState.
    void renderFocusDashboard(const FocusState& fs);

    /// Flash a mode-name banner in the lower strip for ~2 s.
    void showModeToast(const char* modeName);

    // -------------------------------------------------------------------------
    // Special blocking sequences
    // -------------------------------------------------------------------------
    /// Three rapid SURPRISED blink cycles with ILLUM_GREEN_WIN glow.
    void playCelebration();

private:
    // ---- TFT handle ---------------------------------------------------------
    TFT_eSPI tft;

    // ---- Expression & illumination state ------------------------------------
    EyeExpression     currentExpr;
    EyeExpression     targetExpr;
    IlluminationState illumState;

    // ---- Eye geometry  (pixels, NEUTRAL reference) --------------------------
    float eyeW;        ///< Rectangle width  (default 74)
    float eyeH;        ///< Rectangle height, full-open  (default 90)
    float eyeR;        ///< Corner radius  (default 28)
    float eyeSpacing;  ///< Inner-edge gap between the two eyes  (default 40)

    // ---- Look / saccade state -----------------------------------------------
    float lookX;        ///< Current interpolated normalised X  (−1..+1)
    float lookY;        ///< Current interpolated normalised Y  (−1..+1)
    float targetLookX;  ///< Desired normalised X (set via setLookTarget)
    float targetLookY;  ///< Desired normalised Y

    uint32_t nextSaccadeMs;  ///< millis() for next autonomous eye movement

    // ---- Blink state --------------------------------------------------------
    uint32_t nextBlinkTime;  ///< millis() at which the next blink should begin
    bool     isBlinking;
    float    blinkT;         ///< Phase  0..1  through the sine blink curve
    float    eyelidScale;    ///< Effective height multiplier  0 (closed)..1 (open)

    // ---- Pulse animation (ILLUM_PULSE_THINKING) ----------------------------
    float pulseT;    ///< Current brightness factor  0..1
    float pulseDir;  ///< Direction: +1.0 or −1.0 (bouncing)
    bool  doPulse;   ///< True when thinking pulse is active

    // ---- Timing -------------------------------------------------------------
    uint32_t lastAnimMs;    ///< millis() stamp of the last updateAnimation() call
    uint32_t exprStartMs;   ///< millis() when currentExpr was last set
    uint32_t toastExpireMs; ///< millis() after which the mode toast is hidden

    // =========================================================================
    // Private rendering helpers
    // =========================================================================

    /// Return the 565-format RGB colour for the current IlluminationState.
    /// Applies pulseT brightness for ILLUM_PULSE_THINKING.
    uint16_t primaryColor();

    /// Slightly brighter accent colour used for the outer glow ring.
    uint16_t glowColor();

    // ---- Per-expression draw routines ---------------------------------------

    /// Standard glowing rounded-rect eye with halo, dark pupil, and specular.
    ///  lx, ly = interpolated look offset in (−1..+1) normalised space.
    void drawGlowEye(int cx, int cy, float w, float h, float r,
                     uint16_t col, float lx, float ly);

    /// Upper crescent  ( ^ )  — happy, moderate.
    void drawHappyEye(int cx, int cy, float w, float h, uint16_t col);

    /// Extra-wide upper crescent — glee / extreme happiness.
    void drawGleeEye(int cx, int cy, float w, float h, uint16_t col);

    /// Lower crescent  ( _ )  — sad, drooping.
    void drawSadDownEye(int cx, int cy, float w, float h, uint16_t col);

    /// Drooping rectangle with pupils pointed upward toward the user.
    void drawSadUpEye(int cx, int cy, float w, float h, uint16_t col);

    /// Trapezoid with raised inner corners — worried expression.
    void drawWorriedEye(int cx, int cy, float w, float h, uint16_t col);

    /// Magenta heart: two circles + downward-pointing triangle.
    void drawHeartEye(int cx, int cy, float w, uint16_t col);

    /// Flat eye with diagonal brow lines — angry.
    /// leftSide: if true, brow slopes inward-down (mirrored for right eye).
    void drawAngryEye(int cx, int cy, float w, float h, uint16_t col, bool leftSide);

    /// Even flatter than ANGRY with more severe brow — furious.
    void drawFuriousEye(int cx, int cy, float w, float h, uint16_t col, bool leftSide);

    /// Flat amber rectangle + two crossing diagonal lines  (×)  — X-eye.
    void drawXEye(int cx, int cy, float w, float h, uint16_t col);

    /// Single horizontal-line wink (closed-eye slit).
    void drawWinkEye(int cx, int cy, float w);

    /// Thin horizontal slit near the top of the eye frame — blink-high.
    void drawBlinkHighEye(int cx, int cy, float w, uint16_t col);

    /// Thin horizontal slit near the bottom of the eye frame — blink-low.
    void drawBlinkLowEye(int cx, int cy, float w, uint16_t col);

    /// Asymmetric eye pair helper — left normal height, right squinted.
    /// Called for EXPR_SKEPTIC and EXPR_SUSPICIOUS.
    void drawAsymmetricEyes(int lcx, int rcx, int cy,
                            float lh, float rh, uint16_t col);

    /// Open eye with pupil displaced to the upper portion — scared.
    void drawScaredEye(int cx, int cy, float w, float h, uint16_t col);

    /// Top-level renderer: fills background and draws both eyes for currentExpr.
    void renderEyes();
};
