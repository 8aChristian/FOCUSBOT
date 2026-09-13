#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "pinout.h"
#include "robot_types.h"

class DisplayEngine {
public:
    DisplayEngine();
    bool init();
    void showBootDiagnostic(const char* subsystem, bool status, int progressPercent);
    void playWakeupAnimation();
    void setExpression(EyeExpression expr);
    void updateAnimation();
    void renderFocusDashboard(int focusScore, int workSeconds, bool isProcrastinating);
    void showModeToast(const char* modeName);

private:
    TFT_eSPI tft;
    EyeExpression currentExpr;
    EyeExpression targetExpr;
    
    // Procedural eye state
    float eyeWidth;
    float eyeHeight;
    float eyeRound;
    float eyeSpacing;
    float pupilOffsetX;
    float pupilOffsetY;
    float eyelidOpen; // 0.0 (closed) to 1.0 (fully open)
    
    uint32_t lastAnimTime;
    uint32_t nextBlinkTime;
    bool isBlinking;
    float blinkProgress;
    
    void drawEye(int cx, int cy, float w, float h, float r, EyeExpression expr, float lookX, float lookY);
};
