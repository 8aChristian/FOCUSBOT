#include "display_engine.h"

// Color Palette (565 RGB)
#define COLOR_BG          0x0000 // Deep OLED Black
#define COLOR_EYE_CYAN    0x07FF // FocusBot Cyan Glow
#define COLOR_EYE_PUPIL   0x0010 // Dark pupil center
#define COLOR_EYE_FOCUS   0xFD20 // Warm Amber/Orange for Focus Mode
#define COLOR_EYE_HAPPY   0x07E0 // Bright Emerald Green
#define COLOR_EYE_HEART   0xF81F // Neon Magenta / Pink
#define COLOR_EYE_ALERT   0xF800 // Alert Red
#define COLOR_TEXT_MUTED  0x7BEF // Muted Gray

DisplayEngine::DisplayEngine() 
    : currentExpr(EXPR_NEUTRAL), targetExpr(EXPR_NEUTRAL),
      eyeWidth(68.0f), eyeHeight(88.0f), eyeRound(24.0f), eyeSpacing(38.0f),
      pupilOffsetX(0.0f), pupilOffsetY(0.0f), eyelidOpen(1.0f),
      lastAnimTime(0), nextBlinkTime(3000), isBlinking(false), blinkProgress(0.0f) {
}

bool DisplayEngine::init() {
    tft.init();
    tft.setRotation(1); // Landscape 320x240
    tft.fillScreen(COLOR_BG);
    return true;
}

void DisplayEngine::showBootDiagnostic(const char* subsystem, bool status, int progressPercent) {
    tft.setTextFont(2);
    tft.setTextColor(COLOR_EYE_CYAN, COLOR_BG);
    
    // Header
    tft.setCursor(15, 15);
    tft.print(">> FOCUSBOT BIOS v12.0 BOOTLOADER");
    
    // Progress Bar Outer
    int barX = 20;
    int barY = 190;
    int barW = 280;
    int barH = 14;
    tft.drawRoundRect(barX, barY, barW, barH, 4, COLOR_EYE_CYAN);
    
    // Fill Progress
    int fillW = (barW - 4) * progressPercent / 100;
    if (fillW > 0) {
        tft.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, COLOR_EYE_CYAN);
    }
    
    // Subsystem Line
    int lineY = 60 + (progressPercent / 15) * 18;
    tft.setCursor(20, lineY);
    tft.setTextColor(status ? 0x07E0 : 0xF800, COLOR_BG);
    tft.printf("[%s] %-24s", status ? "  OK  " : " FAIL ", subsystem);
    
    delay(80);
}

void DisplayEngine::playWakeupAnimation() {
    tft.fillScreen(COLOR_BG);
    
    // Gradual eye awakening from narrow slit to bright wide awake
    for (float open = 0.05f; open <= 1.0f; open += 0.05f) {
        eyelidOpen = open;
        setExpression(EXPR_DROWSY);
        updateAnimation();
        delay(35);
    }
    
    // Quick curious double-blink upon fully waking
    delay(200);
    setExpression(EXPR_SURPRISED);
    updateAnimation();
    delay(300);
    
    setExpression(EXPR_HAPPY);
    updateAnimation();
    delay(400);
    
    setExpression(EXPR_NEUTRAL);
    updateAnimation();
}

void DisplayEngine::setExpression(EyeExpression expr) {
    targetExpr = expr;
    currentExpr = expr;
}

void DisplayEngine::updateAnimation() {
    uint32_t now = millis();
    
    // Natural periodic blinking
    if (!isBlinking && now > nextBlinkTime) {
        isBlinking = true;
        blinkProgress = 0.0f;
    }
    
    if (isBlinking) {
        blinkProgress += 0.25f;
        if (blinkProgress >= 1.0f) {
            isBlinking = false;
            blinkProgress = 0.0f;
            nextBlinkTime = now + random(2500, 6000);
        }
    }
    
    // Look-around saccades in Default Mode
    if (currentExpr == EXPR_NEUTRAL && !isBlinking) {
        if (random(100) < 3) {
            pupilOffsetX = random(-14, 15);
            pupilOffsetY = random(-10, 11);
        }
    } else {
        pupilOffsetX = 0;
        pupilOffsetY = 0;
    }
    
    // Calculate current effective eyelid scale
    float currentScale = eyelidOpen;
    if (isBlinking) {
        // Sine wave dip for smooth blink
        float factor = sin(blinkProgress * PI);
        currentScale = eyelidOpen * (1.0f - factor * 0.95f);
    }
    
    // Render eyes
    int screenW = 320;
    int screenH = 240;
    int centerY = screenH / 2;
    
    int leftCenterX = (screenW / 2) - (eyeWidth / 2) - (eyeSpacing / 2);
    int rightCenterX = (screenW / 2) + (eyeWidth / 2) + (eyeSpacing / 2);
    
    // Clear display buffer background
    tft.fillScreen(COLOR_BG);
    
    drawEye(leftCenterX, centerY, eyeWidth, eyeHeight * currentScale, eyeRound, currentExpr, pupilOffsetX, pupilOffsetY);
    drawEye(rightCenterX, centerY, eyeWidth, eyeHeight * currentScale, eyeRound, currentExpr, pupilOffsetX, pupilOffsetY);
}

void DisplayEngine::drawEye(int cx, int cy, float w, float h, float r, EyeExpression expr, float lookX, float lookY) {
    if (h < 4.0f) {
        // Closed / Slit eye line
        tft.drawFastHLine(cx - w / 2, cy, w, COLOR_EYE_CYAN);
        tft.drawFastHLine(cx - w / 2, cy + 1, w, COLOR_EYE_CYAN);
        return;
    }
    
    uint16_t primaryColor = COLOR_EYE_CYAN;
    
    switch (expr) {
        case EXPR_HAPPY: {
            primaryColor = COLOR_EYE_HAPPY;
            // Happy upward curved eyes (^ ^)
            int arcH = h * 0.8f;
            tft.fillRoundRect(cx - w / 2, cy - arcH / 2, w, arcH, r, primaryColor);
            tft.fillCircle(cx, cy + arcH / 2 + 2, w * 0.55f, COLOR_BG);
            return;
        }
        case EXPR_FOCUS_SQUINT:
            primaryColor = COLOR_EYE_FOCUS;
            h = h * 0.55f; // Intense focused squint
            break;
        case EXPR_HEART: {
            primaryColor = COLOR_EYE_HEART;
            // Cute Heart Eyes
            int hr = w * 0.26f;
            tft.fillCircle(cx - hr, cy - hr / 2, hr, primaryColor);
            tft.fillCircle(cx + hr, cy - hr / 2, hr, primaryColor);
            tft.fillTriangle(cx - hr * 2, cy - hr / 4, cx + hr * 2, cy - hr / 4, cx, cy + hr * 1.8f, primaryColor);
            return;
        }
        case EXPR_ANGRY_ALERT:
            primaryColor = COLOR_EYE_ALERT;
            break;
        case EXPR_SURPRISED:
            w = w * 1.15f;
            h = h * 1.15f;
            break;
        default:
            primaryColor = COLOR_EYE_CYAN;
            break;
    }
    
    // Draw base glowing eye body
    int x0 = cx - w / 2;
    int y0 = cy - h / 2;
    tft.fillRoundRect(x0, y0, w, h, min(r, h / 2.0f), primaryColor);
    
    // Highlight reflection (cute organic gloss spot)
    tft.fillCircle(x0 + w * 0.72f + lookX * 0.4f, y0 + h * 0.25f + lookY * 0.4f, 7, 0xFFFF);
    tft.fillCircle(x0 + w * 0.35f + lookX * 0.4f, y0 + h * 0.65f + lookY * 0.4f, 3, 0xFFFF);
}

void DisplayEngine::renderFocusDashboard(int focusScore, int workSeconds, bool isProcrastinating) {
    // Focus Mode status overlay at top and bottom
    tft.fillRect(0, 0, 320, 26, COLOR_BG);
    tft.setTextFont(2);
    
    if (isProcrastinating) {
        tft.setTextColor(COLOR_EYE_ALERT, COLOR_BG);
        tft.setCursor(10, 4);
        tft.print("! PROCRASTINACION DETECTADA !");
    } else {
        tft.setTextColor(COLOR_EYE_FOCUS, COLOR_BG);
        tft.setCursor(10, 4);
        int mins = workSeconds / 60;
        int secs = workSeconds % 60;
        tft.printf("FOCUS SESSION: %02d:%02d | SCORE: %d%%", mins, secs, focusScore);
    }
}

void DisplayEngine::showModeToast(const char* modeName) {
    tft.fillRect(40, 200, 240, 32, 0x18E3);
    tft.drawRoundRect(40, 200, 240, 32, 6, COLOR_EYE_CYAN);
    tft.setTextColor(0xFFFF, 0x18E3);
    tft.setTextFont(2);
    tft.setCursor(60, 208);
    tft.printf("MODO: %s", modeName);
}
