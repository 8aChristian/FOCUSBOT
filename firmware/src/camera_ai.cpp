// =============================================================================
// camera_ai.cpp  —  FocusBot Vision & AI Analysis Implementation
// =============================================================================
// Frame-differencing pipeline on QQQVGA (160×120) greyscale frames.
// Computes:
//   • per-pixel absolute diff to detect motion / presence
//   • centroid of "active" pixels  →  look-target for ServoNeck / DisplayEngine
//   • blob-size heuristic to guess whether the motion region is face-sized
//   • procrastination flag when motion is continuous but too random/frequent
// =============================================================================
#include "camera_ai.h"

// ---------------------------------------------------------------------------
// Camera pin mapping — TTGO T-Camera / AI-Thinker module (override in pinout.h)
// ---------------------------------------------------------------------------
#ifndef CAM_PIN_PWDN
#  define CAM_PIN_PWDN    -1
#  define CAM_PIN_RESET   -1
#  define CAM_PIN_XCLK    21
#  define CAM_PIN_SIOD    26
#  define CAM_PIN_SIOC    27
#  define CAM_PIN_D7      35
#  define CAM_PIN_D6      34
#  define CAM_PIN_D5      39
#  define CAM_PIN_D4      36
#  define CAM_PIN_D3      19
#  define CAM_PIN_D2      18
#  define CAM_PIN_D1       5
#  define CAM_PIN_D0       4
#  define CAM_PIN_VSYNC   25
#  define CAM_PIN_HREF    23
#  define CAM_PIN_PCLK    22
#endif

// Frame geometry (QQQVGA)
static constexpr int FRAME_W = 160;
static constexpr int FRAME_H = 120;
static constexpr int FRAME_SZ = FRAME_W * FRAME_H;

// Motion thresholds
static constexpr uint8_t  DIFF_THRESH        = 25;   // per-pixel diff to count as active
static constexpr int      PRESENCE_THRESH    = 50;   // min active pixels → person present
static constexpr int      FACE_MIN_PIXELS    = 20;   // blob-size heuristic lower bound
static constexpr int      FACE_MAX_PIXELS    = 800;  // blob-size heuristic upper bound
static constexpr int      PROCRASTINATE_THR  = 60;   // motion score above this = off-task
static constexpr uint32_t ABSENCE_TIMEOUT_MS = 10000; // 10 s absence before warning
static constexpr uint32_t UPDATE_INTERVAL_MS = 200;   // 5 Hz analysis rate

// ---------------------------------------------------------------------------
CameraAI::CameraAI()
    : initialized(false)
    , prevFrameBuffer(nullptr)
    , prevFrameSize(0)
    , lastCheckTime(0)
    , absenceStartTime(0)
    , continuousFocusSec(0)
    , procrastinationWarnings(0)
    , smoothLookX(0.0f)
    , smoothLookY(0.0f)
{}

// ---------------------------------------------------------------------------
bool CameraAI::init() {
    camera_config_t config = {};

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = CAM_PIN_D0;
    config.pin_d1       = CAM_PIN_D1;
    config.pin_d2       = CAM_PIN_D2;
    config.pin_d3       = CAM_PIN_D3;
    config.pin_d4       = CAM_PIN_D4;
    config.pin_d5       = CAM_PIN_D5;
    config.pin_d6       = CAM_PIN_D6;
    config.pin_d7       = CAM_PIN_D7;
    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_pclk     = CAM_PIN_PCLK;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;
    config.pin_sscb_sda = CAM_PIN_SIOD;
    config.pin_sscb_scl = CAM_PIN_SIOC;
    config.pin_pwdn     = CAM_PIN_PWDN;
    config.pin_reset    = CAM_PIN_RESET;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size   = FRAMESIZE_QQVGA;   // 160×120
    config.jpeg_quality = 12;
    config.fb_count     = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[CameraAI] esp_camera_init failed: 0x%x\n", err);
        initialized = false;
        return false;
    }

    // Allocate previous-frame buffer in PSRAM if available, else heap
    prevFrameBuffer = (uint8_t*)ps_malloc(FRAME_SZ);
    if (!prevFrameBuffer) {
        prevFrameBuffer = (uint8_t*)malloc(FRAME_SZ);
    }
    if (!prevFrameBuffer) {
        Serial.println("[CameraAI] Failed to allocate frame buffer");
        initialized = false;
        return false;
    }
    memset(prevFrameBuffer, 0, FRAME_SZ);
    prevFrameSize = FRAME_SZ;

    initialized = true;
    lastCheckTime = millis();
    Serial.println("[CameraAI] OV2640 initialised OK (QQQVGA grayscale)");
    return true;
}

// ---------------------------------------------------------------------------
FocusAnalysisResult CameraAI::updateFocusMonitoring() {
    FocusAnalysisResult res = {};
    res.lookTargetX = smoothLookX;
    res.lookTargetY = smoothLookY;

    if (!initialized) return res;

    uint32_t now = millis();
    if (now - lastCheckTime < UPDATE_INTERVAL_MS) {
        // Return last smoothed values without a new capture
        res.continuousFocusSec = continuousFocusSec;
        return res;
    }
    lastCheckTime = now;

    // -------------------------------------------------------------------------
    // 1. Capture frame
    // -------------------------------------------------------------------------
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[CameraAI] Frame capture failed");
        return res;
    }

    const uint8_t* cur = fb->buf;
    const int      w   = (int)fb->width;
    const int      h   = (int)fb->height;
    const int      sz  = w * h;

    // -------------------------------------------------------------------------
    // 2. Frame differencing + centroid accumulation
    // -------------------------------------------------------------------------
    long sumX        = 0;
    long sumY        = 0;
    int  activePixels = 0;
    long totalDiff   = 0;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx  = y * w + x;
            int diff = abs((int)cur[idx] - (int)prevFrameBuffer[idx]);
            totalDiff += diff;

            if (diff > DIFF_THRESH) {
                sumX += x;
                sumY += y;
                ++activePixels;
            }
        }
    }

    // -------------------------------------------------------------------------
    // 3. Update previous frame buffer
    // -------------------------------------------------------------------------
    memcpy(prevFrameBuffer, cur, sz < (int)prevFrameSize ? sz : prevFrameSize);
    esp_camera_fb_return(fb);

    // -------------------------------------------------------------------------
    // 4. Motion score (0..100)
    // -------------------------------------------------------------------------
    float avgDiff = (sz > 0) ? (float)totalDiff / (float)sz : 0.0f;
    res.motionScore = constrain(avgDiff * 2.0f, 0.0f, 100.0f);

    // -------------------------------------------------------------------------
    // 5. Centroid → look target (-1..+1)
    // -------------------------------------------------------------------------
    if (activePixels > 0) {
        float rawX = (float)sumX / (float)activePixels;   // 0..w
        float rawY = (float)sumY / (float)activePixels;   // 0..h
        float normX = (rawX / (float)w) * 2.0f - 1.0f;   // -1..+1
        float normY = (rawY / (float)h) * 2.0f - 1.0f;   // -1..+1

        // Exponential moving average (alpha = 0.15)
        smoothLookX += 0.15f * (normX - smoothLookX);
        smoothLookY += 0.15f * (normY - smoothLookY);
    } else {
        // Gently drift back toward centre when no motion
        smoothLookX *= 0.95f;
        smoothLookY *= 0.95f;
    }
    res.lookTargetX = smoothLookX;
    res.lookTargetY = smoothLookY;

    // -------------------------------------------------------------------------
    // 6. Face-detection heuristic (blob size)
    // -------------------------------------------------------------------------
    res.faceDetected = (activePixels >= FACE_MIN_PIXELS &&
                        activePixels <= FACE_MAX_PIXELS);

    // -------------------------------------------------------------------------
    // 7. Presence / absence logic
    // -------------------------------------------------------------------------
    res.personPresent = (activePixels >= PRESENCE_THRESH);

    if (res.personPresent) {
        absenceStartTime = now;   // reset absence clock
        continuousFocusSec = (int)((now - absenceStartTime) / 1000);
    } else {
        if (absenceStartTime == 0) absenceStartTime = now;
        uint32_t absentMs = now - absenceStartTime;
        if (absentMs > ABSENCE_TIMEOUT_MS) {
            continuousFocusSec = 0;
        }
    }
    res.continuousFocusSec = continuousFocusSec;

    // -------------------------------------------------------------------------
    // 8. Procrastination heuristic
    //    High motion score but not a clean "focused stillness" → off task
    // -------------------------------------------------------------------------
    res.isProcrastinating = (res.personPresent &&
                             res.motionScore > PROCRASTINATE_THR);

    if (res.isProcrastinating) {
        ++procrastinationWarnings;
    } else {
        if (procrastinationWarnings > 0) --procrastinationWarnings;
    }

    return res;
}

// ---------------------------------------------------------------------------
camera_fb_t* CameraAI::captureSnapshot() {
    if (!initialized) return nullptr;
    return esp_camera_fb_get();
}

void CameraAI::returnSnapshot(camera_fb_t* fb) {
    if (fb) esp_camera_fb_return(fb);
}
