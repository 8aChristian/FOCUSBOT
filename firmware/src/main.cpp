// =============================================================================
// main.cpp  —  FocusBot v2.0 Production Firmware  (PlatformIO / Arduino)
// =============================================================================
//
// Architecture overview:
//   DisplayEngine      — TFT_eSPI ST7789, eye animations, dashboard
//   AudioSynth         — I2S MAX98357A, sound-effect player
//   MotorController    — DRV8833 dual-H-bridge, wander + cute moves
//   ServoNeck          — SG90 smooth servo, choreography sequencer
//   CameraAI           — OV2640 frame-diff vision, centroid eye-tracking
//   VoiceEar           — INMP441 MEMS mic, wakeword + tap detection
//   ConnectivityBridge — BLE UART bridge to companion phone app
//   BatteryMonitor     — LiPo ADC voltage → percentage
//
// Main loop rate: ~50 Hz (20 ms delay)
// Camera AI analysis rate: 5 Hz (internal timer in CameraAI)
// =============================================================================

#include <Arduino.h>
#include "pinout.h"
#include "robot_types.h"
#include "display_engine.h"
#include "audio_synth.h"
#include "motor_controller.h"
#include "servo_neck.h"
#include "camera_ai.h"
#include "voice_ear.h"
#include "connectivity_bridge.h"
#include "battery_monitor.h"
#include "focusbot_llm_persona.h"

// ---------------------------------------------------------------------------
// Global subsystem objects
// ---------------------------------------------------------------------------
DisplayEngine      display;
AudioSynth         audio;
MotorController    motors;
ServoNeck          neck;
CameraAI           camera;
VoiceEar           voice;
ConnectivityBridge bleBridge;
BatteryMonitor     battery;

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------
RobotMode currentMode   = MODE_BOOT_WAKEUP;
uint32_t  modeStartTime = 0;

FocusState focusState = {
    /* score           */ 100,
    /* continuousSec   */   0,
    /* totalFocusSec   */   0,
    /* milestoneMinutes*/   0,
    /* procrastinating */  false,
    /* personPresent   */  true,
    /* lookX           */  0.0f,
    /* lookY           */  0.0f,
    /* faceDetected    */  false,
};

uint32_t lastTelemetryTime = 0;
uint32_t lastCuteMovTime   = 0;   // idle cute-movement cadence timer
uint32_t nextCuteInterval  = 10000; // randomised per-event (8–15 s)

// =============================================================================
// applyIllumForMode()
//   Drive the RGB illumination ring to match the current operating mode and
//   live focus state.  Call after any mode change or score change.
// =============================================================================
void applyIllumForMode() {
    if (battery.getPercentage() < 20) {
        display.setIllumination(ILLUM_DIM_BATTERY);
        return;
    }
    switch (currentMode) {
        case MODE_DEFAULT_EXPLORE:
            display.setIllumination(ILLUM_CYAN_IDLE);
            break;

        case MODE_FOCUS_GUARD:
            if (focusState.procrastinating)
                display.setIllumination(ILLUM_RED_ALERT);
            else if (focusState.continuousSec >= 25 * 60)
                display.setIllumination(ILLUM_GREEN_WIN);
            else
                display.setIllumination(ILLUM_AMBER_FOCUS);
            break;

        case MODE_LLM_COMPANION:
            display.setIllumination(ILLUM_PULSE_THINKING);
            break;

        default:
            break;
    }
}

// =============================================================================
// switchMode()
//   Transition to a new RobotMode, running the appropriate entry animation.
// =============================================================================
void switchMode(RobotMode newMode) {
    if (currentMode == newMode) return;
    currentMode  = newMode;
    modeStartTime = millis();
    applyIllumForMode();

    switch (newMode) {

        // ------------------------------------------------------------------
        case MODE_DEFAULT_EXPLORE:
            display.showModeToast("EXPLORE");
            display.setExpression(EXPR_HAPPY);
            audio.play(SND_HAPPY_CHIRP);
            motors.resetWanderTimer();
            motors.wiggle();
            neck.lookCenter();
            neck.tiltCute();
            break;

        // ------------------------------------------------------------------
        case MODE_FOCUS_GUARD:
            display.showModeToast("FOCUS GUARD");
            display.setExpression(EXPR_FOCUSED);
            audio.play(SND_FOCUS_START);
            motors.stop();
            neck.lookCenter();
            neck.nodYes();
            break;

        // ------------------------------------------------------------------
        case MODE_LLM_COMPANION:
            display.showModeToast("LLM COMPANION");
            display.setExpression(EXPR_SURPRISED);
            audio.play(SND_CONFIRM_BOOP);
            motors.stop();
            neck.tiltCute();
            break;

        // ------------------------------------------------------------------
        case MODE_LOW_POWER:
            display.showModeToast("LOW BATTERY");
            display.setExpression(EXPR_SLEEPY);
            motors.stop();
            neck.lookCenter();
            break;

        default: break;
    }
}

// =============================================================================
// doIdleCuteMovement()
//   Random 8-case cute behaviour executed during explore mode idle.
//   Expressions draw from the full Cozmo library; durations kept short.
// =============================================================================
void doIdleCuteMovement() {
    int r = random(8);
    switch (r) {

        case 0:
            // Skeptical head-tilt — "hmm, interesting…"
            neck.tiltCute();
            display.setExpression(EXPR_SKEPTIC);
            break;

        case 1:
            // Peek around suspiciously
            neck.peekAround();
            display.setExpression(EXPR_SUSPICIOUS);
            break;

        case 2:
            // Happy shimmy with gleeful eyes
            motors.wiggle();
            display.setExpression(EXPR_GLEE);
            audio.play(SND_HAPPY_CHIRP);
            break;

        case 3:
            // Deep in thought — nod while thinking
            neck.nodYes();
            display.setExpression(EXPR_THINKING);
            break;

        case 4:
            // Cheeky wink with a tilt
            display.setExpression(EXPR_WINK);
            neck.tiltCute();
            break;

        case 5:
            // Curious forward nudge with awe
            motors.nudgeForward();
            display.setExpression(EXPR_AWE);
            break;

        case 6:
            // Cute confused: worried + shake no
            display.setExpression(EXPR_WORRIED);
            neck.shakeNo();
            break;

        case 7:
            // Unimpressed → sudden glee + wiggle
            display.setExpression(EXPR_UNIMPRESSED);
            delay(900);
            display.setExpression(EXPR_GLEE);
            motors.wiggle();
            break;
    }
}

// =============================================================================
// handleLlmReaction() — Parse emotional & actuation tags from LLM response
// =============================================================================
void handleLlmReaction(const String& msg) {
    // 1. Emotion & Visual Reactivity
    if (msg.indexOf("[GLEE]") >= 0) {
        display.setIllumination(ILLUM_GREEN_WIN);
        display.setExpression(EXPR_GLEE);
        audio.play(SND_HAPPY_CHIRP);
    } else if (msg.indexOf("[AWE]") >= 0) {
        display.setIllumination(ILLUM_CYAN_IDLE);
        display.setExpression(EXPR_AWE);
        audio.play(SND_CURIOUS_TRILL);
    } else if (msg.indexOf("[HEART]") >= 0) {
        display.setIllumination(ILLUM_PINK_HAPPY);
        display.setExpression(EXPR_HEART);
        audio.play(SND_PETTED_PURR);
    } else if (msg.indexOf("[WINK]") >= 0) {
        display.setIllumination(ILLUM_CYAN_IDLE);
        display.setExpression(EXPR_WINK);
        audio.play(SND_CONFIRM_BOOP);
    } else if (msg.indexOf("[WORRIED]") >= 0) {
        display.setIllumination(ILLUM_AMBER_FOCUS);
        display.setExpression(EXPR_WORRIED);
        audio.play(SND_SAD_WHINE);
    } else if (msg.indexOf("[ANGRY]") >= 0) {
        display.setIllumination(ILLUM_RED_ALERT);
        display.setExpression(EXPR_ANGRY);
        audio.play(SND_PROCRASTINATION_ALERT);
    } else if (msg.indexOf("[SCARED]") >= 0) {
        display.setIllumination(ILLUM_RED_ALERT);
        display.setExpression(EXPR_SCARED);
        audio.play(SND_SAD_WHINE);
    } else if (msg.indexOf("[SAD]") >= 0) {
        display.setIllumination(ILLUM_DIM_BATTERY);
        display.setExpression(EXPR_SAD_DOWN);
        audio.play(SND_SAD_WHINE);
    } else if (msg.indexOf("[THINKING]") >= 0) {
        display.setIllumination(ILLUM_PULSE_THINKING);
        display.setExpression(EXPR_THINKING);
        audio.play(SND_THINKING_HUM);
    } else {
        display.setIllumination(ILLUM_CYAN_IDLE);
        display.setExpression(EXPR_HAPPY);
        audio.play(SND_HAPPY_CHIRP);
    }

    // 2. Mechatronic Physical Reactions
    if (msg.indexOf("[NOD]") >= 0)    neck.nodYes();
    if (msg.indexOf("[SHAKE]") >= 0)  neck.shakeNo();
    if (msg.indexOf("[TILT]") >= 0)   neck.tiltCute();
    if (msg.indexOf("[WIGGLE]") >= 0) motors.wiggle();
    if (msg.indexOf("[SPIN]") >= 0)   motors.spinJoy();
    if (msg.indexOf("[NUDGE]") >= 0)  motors.nudgeForward();
}

// =============================================================================
// setup()
// =============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n====================================");
    Serial.println("  FOCUSBOT v2.0  PRODUCTION FIRMWARE");
    Serial.println("====================================");

    // Boot-diagnostic splash — each subsystem gets a progress bar tick
    display.init();
    display.showBootDiagnostic("ST7789 IPS Display",    true,  15);

    bool audioOk  = audio.init();
    display.showBootDiagnostic("MAX98357A Audio DAC",   audioOk,  30);

    bool batOk    = battery.init();
    display.showBootDiagnostic("LiPo Battery ADC",      batOk,    45);

    bool servoOk  = neck.init();
    display.showBootDiagnostic("SG90 Neck Servo",       servoOk,  60);

    bool motOk    = motors.init();
    display.showBootDiagnostic("DRV8833 Dual Motors",   motOk,    75);

    bool micOk    = voice.init();
    display.showBootDiagnostic("INMP441 MEMS Mic",      micOk,    88);

    bool camOk    = camera.init();
    display.showBootDiagnostic("OV2640 AI Vision",      camOk,   100);

    delay(300);
    bleBridge.init();
    display.showBootDiagnostic("Wi-Fi Cloud Link", bleBridge.isWifiConnected(), 95);

    Serial.println("[FocusBot] All subsystems online — starting wakeup sequence");

    // Wakeup sequence
    audio.play(SND_WAKEUP_CHIME);
    display.playWakeupAnimation();
    neck.tiltCute();
    delay(300);
    motors.wiggle();

    // Enter explore mode
    switchMode(MODE_DEFAULT_EXPLORE);
    lastCuteMovTime  = millis();
    nextCuteInterval = random(8000, 15000);
}

// =============================================================================
// loop()  — 50 Hz main loop
// =============================================================================
void loop() {
    uint32_t now = millis();

    // Always-on subsystem ticks
    battery.update();
    neck.update();           // Non-blocking servo smooth-motion tick

    // ------------------------------------------------------------------
    // BLE Telemetry  (every 1.5 s)
    // ------------------------------------------------------------------
    if (now - lastTelemetryTime > 1500) {
        lastTelemetryTime = now;
        bleBridge.sendTelemetry(currentMode,
                                battery.getPercentage(),
                                focusState.score);
    }

    // ==================================================================
    // VOICE / TOUCH INPUT  (highest priority — checked every loop)
    // ==================================================================
    VoiceEvent vEvent = voice.listen();

    if (vEvent != VOICE_NONE) {
        switch (vEvent) {

            // ---- Pet / tap on the head -----------------------------------
            case VOICE_TAP_HEAD:
                display.setIllumination(ILLUM_PINK_HAPPY);
                display.setExpression(EXPR_HEART);
                audio.play(SND_PETTED_PURR);
                neck.tiltCute();
                delay(1200);
                display.setExpression(EXPR_HAPPY);
                motors.wiggle();
                applyIllumForMode();
                break;

            // ---- Voice commands -----------------------------------------
            case VOICE_COMMAND_FOCUS:
                switchMode(MODE_FOCUS_GUARD);
                break;

            case VOICE_COMMAND_DEFAULT:
                switchMode(MODE_DEFAULT_EXPLORE);
                break;

            // ---- Wakeword ("Hey FocusBot") ------------------------------
            case VOICE_WAKEWORD_DETECTED:
                display.setIllumination(ILLUM_PULSE_THINKING);
                display.setExpression(EXPR_THINKING);
                audio.play(SND_CONFIRM_BOOP);
                neck.tiltCute();
                motors.stop();

                if (bleBridge.isWifiConnected()) {
                    String reply = bleBridge.queryCloudLlm("¡Hola FocusBot! Dime algo tierno y motivador para concentrarme.");
                    handleLlmReaction(reply);
                    Serial.printf("[Cloud LLM] %s\n", reply.c_str());
                } else if (bleBridge.isConnected()) {
                    switchMode(MODE_LLM_COMPANION);
                } else {
                    display.setExpression(EXPR_WINK);
                    audio.play(SND_HAPPY_CHIRP);
                }
                break;

            // ---- Startled by loud noise ----------------------------------
            case VOICE_LOUD_NOISE:
                display.setExpression(EXPR_SCARED);
                motors.backupShyly();
                neck.shakeNo();
                audio.play(SND_CURIOUS_TRILL);
                delay(600);
                display.setExpression(EXPR_WORRIED);
                break;

            default: break;
        }
    }

    // ==================================================================
    // MODE STATE MACHINE
    // ==================================================================
    switch (currentMode) {

        // ----------------------------------------------------------------
        case MODE_DEFAULT_EXPLORE: {
            motors.updateWander();
            neck.scanCuriosity();

            // Randomised idle cute movement every 8–15 s
            if ((now - lastCuteMovTime) > nextCuteInterval) {
                lastCuteMovTime  = now;
                nextCuteInterval = (uint32_t)random(8000, 15000);
                doIdleCuteMovement();
            }

            display.updateAnimation();
            break;
        }

        // ----------------------------------------------------------------
        case MODE_FOCUS_GUARD: {
            FocusAnalysisResult ai = camera.updateFocusMonitoring();
            int elapsedSec = (int)((now - modeStartTime) / 1000UL);

            // Sync AI result → FocusState
            focusState.personPresent   = ai.personPresent;
            focusState.faceDetected    = ai.faceDetected;
            focusState.procrastinating = ai.isProcrastinating;
            focusState.continuousSec   = ai.continuousFocusSec;
            focusState.totalFocusSec   = elapsedSec;
            focusState.lookX           = ai.lookTargetX;
            focusState.lookY           = ai.lookTargetY;

            // Pass centroid to display for eye-tracking
            display.setLookTarget(ai.lookTargetX, ai.lookTargetY);

            // ---- Procrastination detected --------------------------------
            if (ai.isProcrastinating) {
                focusState.score = max(0, focusState.score - 1);
                display.setIllumination(ILLUM_RED_ALERT);
                display.setExpression(EXPR_ANGRY);
                audio.play(SND_PROCRASTINATION_ALERT);
                neck.shakeNo();
                motors.stop();
            }
            // ---- Person absent ------------------------------------------
            else if (!ai.personPresent) {
                display.setIllumination(ILLUM_RED_ALERT);
                display.setExpression(EXPR_WORRIED);
                neck.peekAround();
            }
            // ---- Focused and present ------------------------------------
            else {
                focusState.score = min(100, focusState.score + 1);
                display.setIllumination(ILLUM_AMBER_FOCUS);
                display.setExpression(EXPR_FOCUSED_TRACKING);

                // 25-minute milestone celebration
                int milestoneMins = ai.continuousFocusSec / 1500; // 1500 s = 25 min
                if (milestoneMins > focusState.milestoneMinutes) {
                    focusState.milestoneMinutes = milestoneMins;
                    display.setIllumination(ILLUM_GREEN_WIN);
                    display.setExpression(EXPR_GLEE);
                    delay(400);
                    display.setExpression(EXPR_AWE);
                    display.playCelebration();
                    audio.play(SND_MILESTONE_FANFARE);
                    neck.nodYes();
                    motors.spinJoy();
                }
            }

            display.updateAnimation();
            display.renderFocusDashboard(focusState);
            break;
        }

        // ----------------------------------------------------------------
        case MODE_LLM_COMPANION: {
            display.setIllumination(ILLUM_PULSE_THINKING);
            display.setExpression(EXPR_THINKING);
            neck.tiltCute();

            String llmMsg = bleBridge.pollIncomingLlmMessage();
            if (llmMsg.length() > 0) {
                handleLlmReaction(llmMsg);
                Serial.printf("[LLM] %s\n", llmMsg.c_str());
            } else {
                display.updateAnimation();
            }
            break;
        }

        // ----------------------------------------------------------------
        case MODE_LOW_POWER: {
            // Minimal activity — just keep display alive
            static uint32_t lowBatToggle = 0;
            if (now - lowBatToggle > 3000) {
                lowBatToggle = now;
                // Alternate between sleepy and sad expressions
                static bool toggle = false;
                display.setExpression(toggle ? EXPR_SLEEPY : EXPR_SAD_DOWN);
                toggle = !toggle;
            }
            break;
        }

        default: break;
    }

    // ==================================================================
    // GLOBAL SAFETY — low-battery override
    // ==================================================================
    if (battery.isLowBattery() && currentMode != MODE_LOW_POWER) {
        Serial.println("[FocusBot] Low battery — entering low-power mode");
        display.setIllumination(ILLUM_DIM_BATTERY);
        display.setExpression(EXPR_SLEEPY);
        motors.stop();
        switchMode(MODE_LOW_POWER);
    }

    delay(20); // 50 Hz main loop
}
