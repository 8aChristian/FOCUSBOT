// =============================================================================
// display_engine.cpp  --  FocusBot v12.0
//
// Full Cozmo-style procedural eye renderer for the 320x240 ST7789 IPS display.
// Implements all expressions from the Cozmo emotional-design sheet plus
// FocusBot-specific extras.  See display_engine.h for the full API.
// =============================================================================

#include "display_engine.h"

// ---------------------------------------------------------------------------
// Colour palette  (RGB-565)
// ---------------------------------------------------------------------------
#define COLOR_BG            0x0000   // Pure OLED black
#define COLOR_CYAN          0x07FF   // Default idle cyan
#define COLOR_AMBER         0xFD20   // Warm amber / focus mode
#define COLOR_RED           0xF800   // Alert red
#define COLOR_GREEN         0x07E0   // Celebration / win green
#define COLOR_MAGENTA       0xF81F   // Heart / happy pink
#define COLOR_DIM           0x6180   // Dim blue-grey (low battery)
#define COLOR_PUPIL         0x0208   // Very dark blue-black pupil
#define COLOR_WHITE         0xFFFF   // Specular highlight
#define COLOR_MUTED         0x7BEF   // Muted grey for HUD text

// Screen & HUD geometry
#define HUD_BAR_H           26
#define SCREEN_W            320
#define SCREEN_H            240

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
DisplayEngine::DisplayEngine()
    : currentExpr(EXPR_NEUTRAL),
      targetExpr(EXPR_NEUTRAL),
      illumState(ILLUM_CYAN_IDLE),
      eyeW(74.0f), eyeH(90.0f), eyeR(28.0f), eyeSpacing(40.0f),
      lookX(0.0f), lookY(0.0f),
      targetLookX(0.0f), targetLookY(0.0f),
      nextSaccadeMs(3000),
      nextBlinkTime(3000),
      isBlinking(false),
      blinkT(0.0f),
      eyelidScale(1.0f),
      pulseT(0.0f), pulseDir(1.0f), doPulse(false),
      lastAnimMs(0), exprStartMs(0), toastExpireMs(0)
{}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------
bool DisplayEngine::init() {
    tft.init();
    tft.setRotation(1);         // Landscape: 320 wide x 240 tall
    tft.fillScreen(COLOR_BG);
    tft.setTextDatum(TL_DATUM);
    lastAnimMs  = millis();
    exprStartMs = millis();
    return true;
}

// ---------------------------------------------------------------------------
// Boot diagnostic
// ---------------------------------------------------------------------------
void DisplayEngine::showBootDiagnostic(const char* subsystem,
                                        bool status,
                                        int  progressPercent) {
    tft.setTextFont(2);
    tft.setTextColor(COLOR_CYAN, COLOR_BG);
    tft.setCursor(15, 10);
    tft.print(">> FOCUSBOT BIOS v12.0 BOOTLOADER");

    // Subsystem result line stacked by progress percentage
    int lineY = 38 + (progressPercent / 14) * 18;
    lineY     = constrain(lineY, 38, 168);
    tft.setCursor(20, lineY);
    tft.setTextColor(status ? COLOR_GREEN : COLOR_RED, COLOR_BG);
    tft.printf("  [%s]  %-26s", status ? " OK " : "FAIL", subsystem);

    // Progress bar
    const int barX = 20;
    const int barY = 190;
    const int barW = 280;
    const int barH = 14;
    tft.drawRoundRect(barX, barY, barW, barH, 4, COLOR_CYAN);
    int fillW = (int)((barW - 4) * (progressPercent / 100.0f));
    if (fillW > 0)
        tft.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, COLOR_CYAN);

    // Percentage label
    tft.setTextColor(COLOR_MUTED, COLOR_BG);
    tft.setCursor(barX + barW + 5, barY);
    tft.printf("%3d%%", progressPercent);

    delay(80);
}

// ---------------------------------------------------------------------------
// Wakeup animation  (blocking)
// ---------------------------------------------------------------------------
void DisplayEngine::playWakeupAnimation() {
    tft.fillScreen(COLOR_BG);
    illumState = ILLUM_CYAN_IDLE;

    // Phase 1: grow from slit to full-open sleepy eyes
    for (float open = 0.04f; open <= 1.0f; open += 0.04f) {
        eyelidScale = open;
        currentExpr = EXPR_SLEEPY;
        renderEyes();
        delay(28);
    }
    eyelidScale = 1.0f;
    delay(150);

    // Phase 2: flash SURPRISED
    currentExpr = EXPR_SURPRISED;
    renderEyes();
    delay(280);

    // Phase 3: settle to HAPPY
    currentExpr = EXPR_HAPPY;
    renderEyes();
    delay(380);

    // Phase 4: settle to NEUTRAL
    currentExpr = EXPR_NEUTRAL;
    renderEyes();
}

// ---------------------------------------------------------------------------
// Expression & illumination setters
// ---------------------------------------------------------------------------
void DisplayEngine::setExpression(EyeExpression expr) {
    currentExpr = expr;
    targetExpr  = expr;
    exprStartMs = millis();
}

void DisplayEngine::setIllumination(IlluminationState illum) {
    illumState = illum;
    doPulse    = (illum == ILLUM_PULSE_THINKING);
    if (!doPulse) {
        pulseT   = 1.0f;
        pulseDir = 1.0f;
    }
}

void DisplayEngine::setLookTarget(float nx, float ny) {
    targetLookX = constrain(nx, -1.0f, 1.0f);
    targetLookY = constrain(ny, -1.0f, 1.0f);
}

// ---------------------------------------------------------------------------
// Colour helpers
// ---------------------------------------------------------------------------
uint16_t DisplayEngine::primaryColor() {
    float brightness = doPulse ? (0.35f + 0.65f * pulseT) : 1.0f;

    switch (illumState) {
        case ILLUM_CYAN_IDLE:
        case ILLUM_PULSE_THINKING: {
            // Scale RGB-565 cyan (0x07FF) channels by brightness factor
            uint8_t g = (uint8_t)(63.0f * brightness);   // 6-bit green
            uint8_t b = (uint8_t)(31.0f * brightness);   // 5-bit blue
            return (uint16_t)((g << 5) | b);
        }
        case ILLUM_AMBER_FOCUS:  return 0xFD20;
        case ILLUM_RED_ALERT:    return 0xF800;
        case ILLUM_GREEN_WIN:    return 0x07E0;
        case ILLUM_PINK_HAPPY:   return 0xF81F;
        case ILLUM_DIM_BATTERY:  return 0x6180;
        default:                 return COLOR_CYAN;
    }
}

uint16_t DisplayEngine::glowColor() {
    // Half-brightness of primary for the mid glow ring
    uint16_t c = primaryColor();
    uint8_t r5 = (c >> 11) & 0x1F;
    uint8_t g6 = (c >>  5) & 0x3F;
    uint8_t b5 =  c        & 0x1F;
    return (uint16_t)(((r5 >> 1) << 11) | ((g6 >> 1) << 5) | (b5 >> 1));
}

// ---------------------------------------------------------------------------
// drawGlowEye -- layered glow + main body + pupil + specular
// This is the fundamental building block for most Cozmo-style expressions.
//
//  Layers drawn bottom-up:
//   1. Outer glow halo    fillRoundRect(w+6, h+6, r+3)  colour >> 2  (very dim)
//   2. Mid glow ring      fillRoundRect(w+2, h+2, r+1)  glowColor()  (half)
//   3. Main eye body      fillRoundRect(w,   h,   r  )  col          (full)
//   4. Dark pupil         fillCircle(cx+lx*12, cy+ly*8+h*0.05, w*0.22)
//   5. Specular 1         fillCircle(cx+w*0.28, cy-h*0.22, 6)  white
//   6. Specular 2         fillCircle(cx-w*0.18, cy+h*0.18, 3)  tinted
// ---------------------------------------------------------------------------
void DisplayEngine::drawGlowEye(int cx, int cy, float w, float h, float r,
                                 uint16_t col, float lx, float ly) {
    if (h < 3.0f) {
        // Degenerate -- draw a 2-pixel slit
        tft.drawFastHLine(cx - (int)(w / 2), cy,     (int)w, col);
        tft.drawFastHLine(cx - (int)(w / 2), cy + 1, (int)w, col);
        return;
    }

    int x0 = (int)(cx - w / 2);
    int y0 = (int)(cy - h / 2);
    int iw = (int)w;
    int ih = (int)h;
    int ir = (int)constrain(r, 1.0f, h / 2.0f);

    // Layer 1: outer halo (very dim)
    uint16_t halo = (uint16_t)(col >> 2);
    tft.fillRoundRect(x0 - 3, y0 - 3, iw + 6, ih + 6, ir + 3, halo);

    // Layer 2: mid glow ring
    tft.fillRoundRect(x0 - 1, y0 - 1, iw + 2, ih + 2, ir + 1, glowColor());

    // Layer 3: main eye body
    tft.fillRoundRect(x0, y0, iw, ih, ir, col);

    // Layer 4: dark pupil offset by look target
    int px = (int)(cx + lx * 12.0f);
    int py = (int)(cy + ly *  8.0f + h * 0.05f);
    tft.fillCircle(px, py, (int)(w * 0.22f), COLOR_PUPIL);

    // Layer 5: large specular (upper-right quad)
    tft.fillCircle((int)(cx + w * 0.28f + lx * 3.0f),
                   (int)(cy - h * 0.22f + ly * 2.0f), 6, COLOR_WHITE);

    // Layer 6: small tinted specular (lower-left quad)
    uint16_t s2 = (uint16_t)(col >> 1) | 0x4000;
    tft.fillCircle((int)(cx - w * 0.18f + lx * 2.0f),
                   (int)(cy + h * 0.18f + ly * 2.0f), 3, s2);
}

// ---------------------------------------------------------------------------
// drawHappyEye -- upper crescent  ( ^ )
// Draws a full rounded rect then erases the bottom half with a circle.
// ---------------------------------------------------------------------------
void DisplayEngine::drawHappyEye(int cx, int cy, float w, float h, uint16_t col) {
    int iw   = (int)w;
    int arcH = (int)(h * 0.82f);
    int x0   = cx - iw / 2;
    int y0   = cy - arcH / 2;
    int ir   = (int)(eyeR);

    // Draw upper-filled rounded rect
    tft.fillRoundRect(x0, y0, iw, arcH, ir, col);
    // Erase lower half with background circle to create crescent
    int cutR = (int)(w * 0.57f);
    tft.fillCircle(cx, cy + arcH / 2 + 3, cutR, COLOR_BG);
    // Specular
    tft.fillCircle(cx + (int)(w * 0.26f), y0 + (int)(arcH * 0.22f), 5, COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// drawGleeEye -- extra-wide upper crescent, extreme happiness
// ---------------------------------------------------------------------------
void DisplayEngine::drawGleeEye(int cx, int cy, float w, float h, uint16_t col) {
    float gw = w * 1.10f;
    int iw   = (int)gw;
    int arcH = (int)(h * 0.88f);
    int x0   = cx - iw / 2;
    int y0   = cy - arcH / 2;
    int ir   = (int)(eyeR + 2);

    tft.fillRoundRect(x0, y0, iw, arcH, ir, col);
    int cutR = (int)(gw * 0.53f);
    tft.fillCircle(cx, cy + arcH / 2 + 2, cutR, COLOR_BG);
    tft.fillCircle(cx + (int)(gw * 0.27f), y0 + (int)(arcH * 0.20f), 6, COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// drawSadDownEye -- lower crescent  ( _ )
// Draws full rect then erases top half with a circle.
// ---------------------------------------------------------------------------
void DisplayEngine::drawSadDownEye(int cx, int cy, float w, float h, uint16_t col) {
    int iw   = (int)w;
    int arcH = (int)(h * 0.82f);
    int x0   = cx - iw / 2;
    // Shift the rect down slightly so the crescent sits below centre
    int y0   = (int)(cy - arcH / 4);
    int ir   = (int)(eyeR);

    tft.fillRoundRect(x0, y0, iw, arcH, ir, col);
    // Erase the upper half
    int cutR = (int)(w * 0.57f);
    tft.fillCircle(cx, y0 - cutR / 2 + 5, cutR, COLOR_BG);
}

// ---------------------------------------------------------------------------
// drawSadUpEye -- drooping shape + pupil aimed upward at the user
// ---------------------------------------------------------------------------
void DisplayEngine::drawSadUpEye(int cx, int cy, float w, float h, uint16_t col) {
    drawSadDownEye(cx, cy, w, h, col);
    // Pupil displaced toward the top of the eye frame
    int px = cx;
    int py = (int)(cy - h * 0.18f);
    tft.fillCircle(px, py, (int)(w * 0.20f), COLOR_PUPIL);
    tft.fillCircle(px + (int)(w * 0.12f), py - 4, 4, COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// drawWorriedEye -- trapezoid with black wedge raising inner corners
// ---------------------------------------------------------------------------
void DisplayEngine::drawWorriedEye(int cx, int cy, float w, float h, uint16_t col) {
    float wh = h * 0.75f;
    drawGlowEye(cx, cy, w, wh, eyeR, col, lookX, lookY);

    int ey0 = (int)(cy - wh / 2);
    int tr  = (int)(w * 0.32f);

    // Inner-upper-left black wedge (raises inner left corner visually)
    tft.fillTriangle(
        cx - (int)(w / 2),      ey0,
        cx - (int)(w / 2) + tr, ey0,
        cx - (int)(w / 2),      ey0 + tr,
        COLOR_BG);

    // Inner-upper-right black wedge (raises inner right corner visually)
    tft.fillTriangle(
        cx + (int)(w / 2),      ey0,
        cx + (int)(w / 2) - tr, ey0,
        cx + (int)(w / 2),      ey0 + tr,
        COLOR_BG);
}

// ---------------------------------------------------------------------------
// drawHeartEye -- two circles + downward-pointing triangle in magenta
// ---------------------------------------------------------------------------
void DisplayEngine::drawHeartEye(int cx, int cy, float w, uint16_t col) {
    int hr = (int)(w * 0.26f);
    tft.fillCircle(cx - hr, cy - hr / 2, hr, col);
    tft.fillCircle(cx + hr, cy - hr / 2, hr, col);
    tft.fillTriangle(
        cx - hr * 2,  cy - hr / 4,
        cx + hr * 2,  cy - hr / 4,
        cx,           cy + (int)(hr * 1.85f),
        col);
    // Specular on left lobe
    tft.fillCircle(cx - hr + hr / 3, cy - hr, 3, COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// drawAngryEye -- flat rect + diagonal brow lines
// leftSide: true for left eye (brow slopes outer-high → inner-low)
// ---------------------------------------------------------------------------
void DisplayEngine::drawAngryEye(int cx, int cy, float w, float h,
                                  uint16_t col, bool leftSide) {
    float ah = h * 0.42f;
    int   iw = (int)w;
    int   ih = (int)ah;
    int   x0 = cx - iw / 2;
    int   y0 = (int)(cy - ah / 2);
    int   ir = max(1, (int)(eyeR * 0.50f));

    tft.fillRoundRect(x0, y0, iw, ih, ir, col);
    // Small centred pupil in flat eye
    tft.fillCircle(cx, cy, (int)(w * 0.18f), COLOR_PUPIL);

    // Brow: thick diagonal line above eye
    int browBase = y0 - 6;
    int bx1, by1, bx2, by2;
    if (leftSide) {
        bx1 = cx - (int)(w * 0.52f); by1 = browBase - 5;
        bx2 = cx + (int)(w * 0.52f); by2 = browBase + 5;
    } else {
        bx1 = cx - (int)(w * 0.52f); by1 = browBase + 5;
        bx2 = cx + (int)(w * 0.52f); by2 = browBase - 5;
    }
    for (int t = -2; t <= 2; t++)
        tft.drawLine(bx1, by1 + t, bx2, by2 + t, col);
}

// ---------------------------------------------------------------------------
// drawFuriousEye -- more severe than ANGRY: flatter + steeper brow
// ---------------------------------------------------------------------------
void DisplayEngine::drawFuriousEye(int cx, int cy, float w, float h,
                                    uint16_t col, bool leftSide) {
    float ah = h * 0.30f;
    int   iw = (int)w;
    int   ih = (int)ah;
    int   x0 = cx - iw / 2;
    int   y0 = (int)(cy - ah / 2);
    int   ir = max(1, (int)(eyeR * 0.35f));

    tft.fillRoundRect(x0, y0, iw, ih, ir, col);
    tft.fillCircle(cx, cy, (int)(w * 0.14f), COLOR_PUPIL);

    int browBase = y0 - 8;
    int bx1, by1, bx2, by2;
    if (leftSide) {
        bx1 = cx - (int)(w * 0.55f); by1 = browBase - 9;
        bx2 = cx + (int)(w * 0.55f); by2 = browBase + 7;
    } else {
        bx1 = cx - (int)(w * 0.55f); by1 = browBase + 7;
        bx2 = cx + (int)(w * 0.55f); by2 = browBase - 9;
    }
    for (int t = -3; t <= 3; t++)
        tft.drawLine(bx1, by1 + t, bx2, by2 + t, col);
}

// ---------------------------------------------------------------------------
// drawXEye -- flat amber rectangle + two crossing diagonal lines
// ---------------------------------------------------------------------------
void DisplayEngine::drawXEye(int cx, int cy, float w, float h, uint16_t col) {
    float xh = h * 0.40f;
    int   iw = (int)w;
    int   ih = (int)xh;
    int   x0 = cx - iw / 2;
    int   y0 = (int)(cy - xh / 2);

    tft.fillRoundRect(x0, y0, iw, ih, max(1, (int)(eyeR * 0.4f)), col);

    // Two thick background-coloured diagonals forming an X
    for (int t = -2; t <= 2; t++) {
        tft.drawLine(x0 + 6, y0 + 4 + t,       x0 + iw - 6, y0 + ih - 4 + t, COLOR_BG);
        tft.drawLine(x0 + 6, y0 + ih - 4 + t,  x0 + iw - 6, y0 + 4 + t,      COLOR_BG);
    }
}

// ---------------------------------------------------------------------------
// drawWinkEye -- single 3-pixel-thick horizontal slit (closed eye)
// ---------------------------------------------------------------------------
void DisplayEngine::drawWinkEye(int cx, int cy, float w) {
    uint16_t col = primaryColor();
    int iw = (int)w;
    for (int t = -1; t <= 1; t++)
        tft.drawFastHLine(cx - iw / 2, cy + t, iw, col);
}

// ---------------------------------------------------------------------------
// drawBlinkHighEye -- thin slit near the top of the eye frame
// ---------------------------------------------------------------------------
void DisplayEngine::drawBlinkHighEye(int cx, int cy, float w, uint16_t col) {
    int iw = (int)w;
    int y  = (int)(cy - eyeH * 0.28f);
    for (int t = -2; t <= 2; t++)
        tft.drawFastHLine(cx - iw / 2, y + t, iw, col);
}

// ---------------------------------------------------------------------------
// drawBlinkLowEye -- thin slit near the bottom of the eye frame
// ---------------------------------------------------------------------------
void DisplayEngine::drawBlinkLowEye(int cx, int cy, float w, uint16_t col) {
    int iw = (int)w;
    int y  = (int)(cy + eyeH * 0.28f);
    for (int t = -2; t <= 2; t++)
        tft.drawFastHLine(cx - iw / 2, y + t, iw, col);
}

// ---------------------------------------------------------------------------
// drawScaredEye -- large open eye with pupil displaced to upper portion
// ---------------------------------------------------------------------------
void DisplayEngine::drawScaredEye(int cx, int cy, float w, float h, uint16_t col) {
    int iw = (int)w;
    int ih = (int)h;
    int x0 = cx - iw / 2;
    int y0 = cy  - ih / 2;
    int ir = (int)constrain(eyeR, 1.0f, h / 2.0f);

    // Glow layers
    uint16_t halo = (uint16_t)(col >> 2);
    tft.fillRoundRect(x0 - 3, y0 - 3, iw + 6, ih + 6, ir + 3, halo);
    tft.fillRoundRect(x0 - 1, y0 - 1, iw + 2, ih + 2, ir + 1, glowColor());
    tft.fillRoundRect(x0,     y0,     iw,     ih,     ir,     col);

    // Pupil displaced toward the top-centre
    int px = cx;
    int py = (int)(cy - h * 0.28f);
    tft.fillCircle(px, py, (int)(w * 0.22f), COLOR_PUPIL);
    tft.fillCircle(px + 4, py - 4, 5, COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// drawAsymmetricEyes -- one eye at lh, other at rh (SKEPTIC / SUSPICIOUS)
// ---------------------------------------------------------------------------
void DisplayEngine::drawAsymmetricEyes(int lcx, int rcx, int cy,
                                        float lh, float rh, uint16_t col) {
    drawGlowEye(lcx, cy, eyeW, lh, eyeR, col, lookX, lookY);
    drawGlowEye(rcx, cy, eyeW, rh, eyeR, col, lookX, lookY);
}

// ---------------------------------------------------------------------------
// renderEyes -- master expression dispatcher
// Clears the screen then draws both eyes for the current expression + state.
// ---------------------------------------------------------------------------
void DisplayEngine::renderEyes() {
    tft.fillScreen(COLOR_BG);

    uint16_t col = primaryColor();
    int      cy  = SCREEN_H / 2;                             // 120
    int      lcx = (int)(SCREEN_W / 2 - eyeW / 2 - eyeSpacing / 2); // ~103
    int      rcx = (int)(SCREEN_W / 2 + eyeW / 2 + eyeSpacing / 2); // ~217
    float    cH  = eyeH * eyelidScale;  // height modulated by blink

    switch (currentExpr) {

        // ----------------------------------------------------------------
        case EXPR_NEUTRAL:
        case EXPR_THINKING:
        case EXPR_FOCUSED_TRACKING:
            drawGlowEye(lcx, cy, eyeW, cH, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy, eyeW, cH, eyeR, col, lookX, lookY);
            break;

        // ----------------------------------------------------------------
        case EXPR_BLINK_HIGH:
            drawBlinkHighEye(lcx, cy, eyeW, col);
            drawBlinkHighEye(rcx, cy, eyeW, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_BLINK_LOW:
            drawBlinkLowEye(lcx, cy, eyeW, col);
            drawBlinkLowEye(rcx, cy, eyeW, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_HAPPY:
            drawHappyEye(lcx, cy, eyeW, eyeH, col);
            drawHappyEye(rcx, cy, eyeW, eyeH, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_GLEE:
            drawGleeEye(lcx, cy, eyeW, eyeH, col);
            drawGleeEye(rcx, cy, eyeW, eyeH, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_SAD_DOWN:
            drawSadDownEye(lcx, cy, eyeW, cH, col);
            drawSadDownEye(rcx, cy, eyeW, cH, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_SAD_UP:
            drawSadUpEye(lcx, cy, eyeW, cH, col);
            drawSadUpEye(rcx, cy, eyeW, cH, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_WORRIED:
            drawWorriedEye(lcx, cy, eyeW, eyeH, col);
            drawWorriedEye(rcx, cy, eyeW, eyeH, col);
            break;

        // ----------------------------------------------------------------
        // EXPR_FOCUSED == EXPR_FOCUS_SQUINT (legacy alias in enum)
        case EXPR_FOCUSED:
            drawGlowEye(lcx, cy, eyeW, eyeH * 0.52f * eyelidScale, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy, eyeW, eyeH * 0.52f * eyelidScale, eyeR, col, lookX, lookY);
            break;

        // ----------------------------------------------------------------
        case EXPR_ANNOYED: {
            // Asymmetric vertical positions: one eye 7px lower
            int off = 7;
            drawGlowEye(lcx, cy + off, eyeW, eyeH * 0.65f, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy,       eyeW, eyeH * 0.65f, eyeR, col, lookX, lookY);
            break;
        }

        // ----------------------------------------------------------------
        case EXPR_SURPRISED: {
            float sw = eyeW * 1.20f;
            float sh = eyeH * 1.20f * eyelidScale;
            drawGlowEye(lcx, cy, sw, sh, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy, sw, sh, eyeR, col, lookX, lookY);
            break;
        }

        // ----------------------------------------------------------------
        case EXPR_SKEPTIC:
            // Left = normal, right = squinted  (side-eye)
            drawAsymmetricEyes(lcx, rcx, cy, eyeH * eyelidScale, eyeH * 0.45f, col);
            break;

        // ----------------------------------------------------------------
        case EXPR_FRUSTRATED:
            drawGlowEye(lcx, cy, eyeW, eyeH * 0.50f * eyelidScale, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy, eyeW, eyeH * 0.50f * eyelidScale, eyeR, col, lookX, lookY);
            break;

        // ----------------------------------------------------------------
        case EXPR_UNIMPRESSED:
            drawGlowEye(lcx, cy, eyeW, eyeH * 0.24f, max(1.0f, eyeR * 0.3f), col, 0, 0);
            drawGlowEye(rcx, cy, eyeW, eyeH * 0.24f, max(1.0f, eyeR * 0.3f), col, 0, 0);
            break;

        // ----------------------------------------------------------------
        // EXPR_SLEEPY == EXPR_DROWSY (enum alias)
        case EXPR_SLEEPY:
            drawGlowEye(lcx, cy + 5, eyeW, eyeH * 0.48f * eyelidScale, eyeR, col, 0.0f, 0.3f);
            drawGlowEye(rcx, cy + 5, eyeW, eyeH * 0.48f * eyelidScale, eyeR, col, 0.0f, 0.3f);
            break;

        // ----------------------------------------------------------------
        case EXPR_SUSPICIOUS:
            // Right eye squinted, both shifted slightly left
            drawGlowEye(lcx, cy, eyeW, eyeH * eyelidScale, eyeR, col, -0.30f, lookY);
            drawGlowEye(rcx, cy, eyeW, eyeH * 0.42f,       eyeR, col, -0.30f, lookY);
            break;

        // ----------------------------------------------------------------
        case EXPR_SQUINT:
            drawGlowEye(lcx, cy, eyeW, eyeH * 0.30f, max(2.0f, eyeR * 0.4f), col, lookX, lookY);
            drawGlowEye(rcx, cy, eyeW, eyeH * 0.30f, max(2.0f, eyeR * 0.4f), col, lookX, lookY);
            break;

        // ----------------------------------------------------------------
        // EXPR_ANGRY  (alias: EXPR_ANGRY_ALERT removed -- use EXPR_FURIOUS)
        case EXPR_ANGRY:
            drawAngryEye(lcx, cy, eyeW, eyeH, col, true);
            drawAngryEye(rcx, cy, eyeW, eyeH, col, false);
            break;

        // ----------------------------------------------------------------
        // EXPR_FURIOUS == EXPR_ANGRY_ALERT (legacy alias in enum)
        case EXPR_FURIOUS:
            drawFuriousEye(lcx, cy, eyeW, eyeH, COLOR_RED, true);
            drawFuriousEye(rcx, cy, eyeW, eyeH, COLOR_RED, false);
            break;

        // ----------------------------------------------------------------
        case EXPR_SCARED: {
            float sw = eyeW * 1.15f;
            float sh = eyeH * 1.15f * eyelidScale;
            drawScaredEye(lcx, cy, sw, sh, col);
            drawScaredEye(rcx, cy, sw, sh, col);
            break;
        }

        // ----------------------------------------------------------------
        case EXPR_AWE: {
            float aw = eyeW * 1.40f;
            float ah = eyeH * 1.40f * eyelidScale;
            drawGlowEye(lcx, cy, aw, ah, eyeR + 4, col, lookX, lookY);
            drawGlowEye(rcx, cy, aw, ah, eyeR + 4, col, lookX, lookY);
            break;
        }

        // ----------------------------------------------------------------
        case EXPR_HEART:
            drawHeartEye(lcx, cy, eyeW, COLOR_MAGENTA);
            drawHeartEye(rcx, cy, eyeW, COLOR_MAGENTA);
            break;

        // ----------------------------------------------------------------
        case EXPR_WINK:
            drawGlowEye(lcx, cy, eyeW, cH, eyeR, col, lookX, lookY);
            drawWinkEye(rcx, cy, eyeW);
            break;

        // ----------------------------------------------------------------
        case EXPR_CELEBRATE: {
            float sw = eyeW * 1.20f;
            float sh = eyeH * 1.20f * eyelidScale;
            drawGlowEye(lcx, cy, sw, sh, eyeR, COLOR_GREEN, lookX, lookY);
            drawGlowEye(rcx, cy, sw, sh, eyeR, COLOR_GREEN, lookX, lookY);
            break;
        }

        // ----------------------------------------------------------------
        case EXPR_ABSENT_ALERT:
            // Wide eyes scanning left for a missing user
            drawGlowEye(lcx, cy, eyeW * 1.05f, cH, eyeR, col, -0.70f, lookY);
            drawGlowEye(rcx, cy, eyeW * 1.05f, cH, eyeR, col, -0.70f, lookY);
            break;

        // ----------------------------------------------------------------
        case EXPR_LOW_BATTERY:
            drawGlowEye(lcx, cy + 6, eyeW, eyeH * 0.35f, eyeR, COLOR_DIM, 0, 0.20f);
            drawGlowEye(rcx, cy + 6, eyeW, eyeH * 0.35f, eyeR, COLOR_DIM, 0, 0.20f);
            break;

        // ----------------------------------------------------------------
        default:
            drawGlowEye(lcx, cy, eyeW, cH, eyeR, col, lookX, lookY);
            drawGlowEye(rcx, cy, eyeW, cH, eyeR, col, lookX, lookY);
            break;
    }
}

// ---------------------------------------------------------------------------
// updateAnimation -- 50 Hz animation pump
//
// Call this every loop() tick.  Handles:
//   1. Look-target lerp (factor 0.12 per tick)
//   2. Autonomous saccades in NEUTRAL / THINKING (every 2.5..4.5 s)
//   3. Auto-blink with sine-curve eyelid scale
//   4. Thinking pulse (ILLUM_PULSE_THINKING, ~1.5 Hz bounce)
//   5. Calls renderEyes()
// ---------------------------------------------------------------------------
void DisplayEngine::updateAnimation() {
    uint32_t now = millis();
    float    dt  = (float)(now - lastAnimMs) / 1000.0f;
    if (dt <= 0.001f) dt = 0.020f;   // guard against zero-delta
    lastAnimMs = now;

    // --- 1. Smooth look interpolation ---
    const float LERP = 0.12f;
    lookX += (targetLookX - lookX) * LERP;
    lookY += (targetLookY - lookY) * LERP;

    // --- 2. Autonomous saccades ---
    if ((currentExpr == EXPR_NEUTRAL ||
         currentExpr == EXPR_THINKING) && now >= nextSaccadeMs) {
        targetLookX   = (float)(random(-60, 61)) / 100.0f;
        targetLookY   = (float)(random(-60, 61)) / 100.0f;
        nextSaccadeMs = now + (uint32_t)random(2500, 4500);
    }

    // --- 3. Auto-blink ---
    if (!isBlinking && now >= nextBlinkTime) {
        isBlinking = true;
        blinkT     = 0.0f;
    }

    if (isBlinking) {
        blinkT += dt * 6.0f;    // ~1 s full sine arc
        if (blinkT >= 1.0f) {
            isBlinking    = false;
            blinkT        = 0.0f;
            eyelidScale   = 1.0f;
            nextBlinkTime = now + (uint32_t)random(2500, 6000);
        } else {
            // Sine dip:  1 -> 0 -> 1
            eyelidScale = 1.0f - sinf(blinkT * (float)PI);
            eyelidScale = constrain(eyelidScale, 0.04f, 1.0f);
        }
    } else {
        eyelidScale = 1.0f;
    }

    // --- 4. Thinking pulse (1.5 Hz, bounce 0..1) ---
    if (doPulse) {
        // 3.0 step/s means half-cycle is ~0.33 s -> ~1.5 Hz full bounce
        pulseT += pulseDir * dt * 3.0f;
        if (pulseT >= 1.0f) { pulseT = 1.0f; pulseDir = -1.0f; }
        if (pulseT <= 0.0f) { pulseT = 0.0f; pulseDir =  1.0f; }
    }

    // --- 5. Render ---
    renderEyes();
}

// ---------------------------------------------------------------------------
// renderFocusDashboard -- top + bottom HUD strips for Focus Guard mode
// ---------------------------------------------------------------------------
void DisplayEngine::renderFocusDashboard(const FocusState& fs) {
    // ---- Top bar (timer + score) ----
    tft.fillRect(0, 0, SCREEN_W, HUD_BAR_H, COLOR_BG);
    tft.setTextFont(2);

    if (fs.procrastinating) {
        tft.setTextColor(COLOR_RED, COLOR_BG);
        tft.setCursor(8, 5);
        tft.print("!!! PROCRASTINACION DETECTADA !!!");
    } else {
        int mins = fs.totalFocusSec / 60;
        int secs = fs.totalFocusSec % 60;
        tft.setTextColor(COLOR_AMBER, COLOR_BG);
        tft.setCursor(6, 5);
        tft.printf("FOCUS %02d:%02d", mins, secs);

        tft.setTextColor(COLOR_GREEN, COLOR_BG);
        tft.setCursor(212, 5);
        tft.printf("SCORE %3d%%", fs.score);
    }
    tft.drawFastHLine(0, HUD_BAR_H, SCREEN_W, COLOR_MUTED);

    // ---- Bottom bar (focus score strip) ----
    int bottomY = SCREEN_H - HUD_BAR_H;
    tft.fillRect(0, bottomY, SCREEN_W, HUD_BAR_H, COLOR_BG);
    tft.drawFastHLine(0, bottomY, SCREEN_W, COLOR_MUTED);

    // Proportional fill bar
    int fillW = (int)((SCREEN_W - 4) * (fs.score / 100.0f));
    if (fillW > 0) {
        uint16_t barCol = fs.procrastinating ? COLOR_RED : COLOR_GREEN;
        tft.fillRoundRect(2, bottomY + 4, fillW, HUD_BAR_H - 8, 3, barCol);
    }

    // Milestone label
    if (fs.milestoneMinutes > 0) {
        tft.setTextColor(COLOR_WHITE, COLOR_BG);
        tft.setCursor(SCREEN_W - 80, bottomY + 6);
        tft.printf("+%dmin!", fs.milestoneMinutes);
    }
}

// ---------------------------------------------------------------------------
// showModeToast -- flash a mode-name banner in the bottom strip for ~2 s
// ---------------------------------------------------------------------------
void DisplayEngine::showModeToast(const char* modeName) {
    const int toastY = SCREEN_H - 38;
    tft.fillRoundRect(34, toastY, SCREEN_W - 68, 32, 6, 0x18E3);
    tft.drawRoundRect(34, toastY, SCREEN_W - 68, 32, 6, COLOR_CYAN);
    tft.setTextColor(COLOR_WHITE, 0x18E3);
    tft.setTextFont(2);
    tft.setCursor(52, toastY + 9);
    tft.printf("MODO: %s", modeName);
    toastExpireMs = millis() + 2000;
}

// ---------------------------------------------------------------------------
// playCelebration -- 3 rapid SURPRISED blink cycles with ILLUM_GREEN_WIN
// ---------------------------------------------------------------------------
void DisplayEngine::playCelebration() {
    IlluminationState savedIllum = illumState;
    EyeExpression     savedExpr  = currentExpr;

    setIllumination(ILLUM_GREEN_WIN);
    currentExpr = EXPR_CELEBRATE;

    for (int i = 0; i < 3; i++) {
        // Full-open frame
        eyelidScale = 1.0f;
        renderEyes();
        delay(130);

        // Rapid squint blink
        eyelidScale = 0.08f;
        renderEyes();
        delay(75);

        // Re-open
        eyelidScale = 1.0f;
        renderEyes();
        delay(130);
    }

    // Restore previous state
    setIllumination(savedIllum);
    setExpression(savedExpr);
    eyelidScale = 1.0f;
}
