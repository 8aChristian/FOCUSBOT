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

// Subsystem Instances
DisplayEngine      display;
AudioSynth         audio;
MotorController    motors;
ServoNeck          neck;
CameraAI           camera;
VoiceEar           voice;
ConnectivityBridge bleBridge;
BatteryMonitor     battery;

// Robot State
RobotMode currentMode = MODE_BOOT_WAKEUP;
uint32_t modeStartTime = 0;
int currentFocusScore = 100;
uint32_t lastTelemetryTime = 0;

void switchMode(RobotMode newMode) {
    if (currentMode == newMode) return;
    currentMode = newMode;
    modeStartTime = millis();

    switch (newMode) {
        case MODE_DEFAULT_EXPLORE:
            display.showModeToast("DEFAULT EXPLORE");
            audio.play(SND_HAPPY_CHIRP);
            display.setExpression(EXPR_HAPPY);
            motors.resetWanderTimer();
            neck.lookCenter();
            break;

        case MODE_FOCUS_GUARD:
            display.showModeToast("FOCUS MODE (AI)");
            motors.stop();
            neck.lookCenter();
            audio.play(SND_FOCUS_START);
            display.setExpression(EXPR_FOCUS_SQUINT);
            break;

        case MODE_LLM_COMPANION:
            display.showModeToast("LLM COMPANION");
            motors.stop();
            audio.play(SND_CONFIRM_BOOP);
            display.setExpression(EXPR_SURPRISED);
            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n==========================================");
    Serial.println(">> FOCUSBOT v12.0 PRODUCTION FIRMWARE <<");
    Serial.println("==========================================");

    // 1. Initialize Display
    display.init();
    display.showBootDiagnostic("ST7789 IPS Display", true, 15);

    // 2. Initialize Audio Synthesizer
    bool audioOk = audio.init();
    display.showBootDiagnostic("MAX98357A Audio DAC", audioOk, 30);

    // 3. Initialize Battery Telemetry
    bool batOk = battery.init();
    display.showBootDiagnostic("LiPo Battery ADC", batOk, 45);

    // 4. Initialize Pan Neck Servo
    bool servoOk = neck.init();
    display.showBootDiagnostic("SG90 Neck Servo", servoOk, 60);

    // 5. Initialize DRV8833 Dual Motors
    bool motOk = motors.init();
    display.showBootDiagnostic("DRV8833 Dual H-Bridge", motOk, 75);

    // 6. Initialize INMP441 Digital Microphone
    bool micOk = voice.init();
    display.showBootDiagnostic("INMP441 MEMS Mic", micOk, 88);

    // 7. Initialize OV2640 AI Camera
    bool camOk = camera.init();
    display.showBootDiagnostic("OV2640 AI Vision", camOk, 100);

    delay(300);

    // 8. Initialize Bluetooth BLE Mobile App Bridge
    bleBridge.init();

    // WAKE UP SEQUENCE!
    Serial.println("[FocusBot] All Subsystems Online! Awakening...");
    audio.play(SND_WAKEUP_CHIME);
    display.playWakeupAnimation();

    // Default to explore mode
    switchMode(MODE_DEFAULT_EXPLORE);
}

void loop() {
    uint32_t now = millis();

    // 1. Update Hardware Watchdogs & Telemetry
    battery.update();
    neck.update();

    // Send status to paired phone every 1.5 seconds
    if (now - lastTelemetryTime > 1500) {
        lastTelemetryTime = now;
        bleBridge.sendTelemetry(currentMode, battery.getPercentage(), currentFocusScore);
    }

    // 2. Micro-Feature: Check Voice Ear & Petting Sensor
    VoiceEvent vEvent = voice.listen();
    if (vEvent != VOICE_NONE) {
        switch (vEvent) {
            case VOICE_TAP_HEAD: // Head Petting Detection!
                display.setExpression(EXPR_HEART);
                audio.play(SND_PETTED_PURR);
                neck.setAngle(random(75, 105), 3.0f);
                delay(1200);
                display.setExpression(EXPR_HAPPY);
                break;

            case VOICE_COMMAND_FOCUS: // "Focus Mode" voice command
                switchMode(MODE_FOCUS_GUARD);
                break;

            case VOICE_COMMAND_DEFAULT: // "Default Mode" voice command
                switchMode(MODE_DEFAULT_EXPLORE);
                break;

            case VOICE_WAKEWORD_DETECTED: // "Hey Focus"
                audio.play(SND_CONFIRM_BOOP);
                display.setExpression(EXPR_SURPRISED);
                motors.stop();
                neck.lookCenter();
                if (bleBridge.isConnected()) {
                    switchMode(MODE_LLM_COMPANION);
                }
                break;

            default:
                break;
        }
    }

    // 3. Mode State Machine
    switch (currentMode) {
        // -------------------------------------------------------------
        // MODE A: DEFAULT EXPLORATION
        // Gentle wander on the desk, random head scanning, cute eyes
        // -------------------------------------------------------------
        case MODE_DEFAULT_EXPLORE: {
            motors.updateWander();
            neck.scanCuriosity();
            display.updateAnimation();
            break;
        }

        // -------------------------------------------------------------
        // MODE B: FOCUS GUARD (AI VISION)
        // Checks presence and anti-procrastination heuristics
        // -------------------------------------------------------------
        case MODE_FOCUS_GUARD: {
            FocusAnalysisResult analysis = camera.updateFocusMonitoring();
            int elapsedSec = (now - modeStartTime) / 1000;

            if (analysis.isProcrastinating) {
                currentFocusScore = max(0, currentFocusScore - 1);
                display.setExpression(EXPR_ANGRY_ALERT);
                display.updateAnimation();
                display.renderFocusDashboard(currentFocusScore, elapsedSec, true);
                audio.play(SND_PROCRASTINATION_ALERT);
                delay(400);
            } else {
                currentFocusScore = min(100, currentFocusScore + 1);
                display.setExpression(EXPR_FOCUS_SQUINT);
                display.updateAnimation();
                display.renderFocusDashboard(currentFocusScore, elapsedSec, false);
            }
            break;
        }

        // -------------------------------------------------------------
        // MODE C: LLM MOBILE APP COMPANION
        // Connected to smartphone with LLM (OpenAI / Claude / Gemini)
        // -------------------------------------------------------------
        case MODE_LLM_COMPANION: {
            String llmMsg = bleBridge.pollIncomingLlmMessage();
            if (llmMsg.length() > 0) {
                // Speech received from LLM
                audio.play(SND_HAPPY_CHIRP);
                display.setExpression(EXPR_HAPPY);
            } else {
                display.updateAnimation();
            }
            break;
        }

        default:
            break;
    }

    // Safety: Low Battery Alert
    if (battery.isLowBattery()) {
        display.setExpression(EXPR_DROWSY);
        motors.stop();
    }

    delay(20); // 50 Hz main control loop
}
