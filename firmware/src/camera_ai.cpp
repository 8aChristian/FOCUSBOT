#include "camera_ai.h"

CameraAI::CameraAI() 
    : initialized(false), prevFrameBuffer(nullptr), prevFrameSize(0),
      lastCheckTime(0), absenceStartTime(0), continuousFocusSec(0),
      procrastinationWarnings(0) {}

bool CameraAI::init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_4;
    config.ledc_timer = LEDC_TIMER_2;
    config.pin_d0 = PIN_CAM_D0;
    config.pin_d1 = PIN_CAM_D1;
    config.pin_d2 = PIN_CAM_D2;
    config.pin_d3 = PIN_CAM_D3;
    config.pin_d4 = PIN_CAM_D4;
    config.pin_d5 = PIN_CAM_D5;
    config.pin_d6 = PIN_CAM_D6;
    config.pin_d7 = PIN_CAM_D7;
    config.pin_xclk = PIN_CAM_XCLK;
    config.pin_pclk = PIN_CAM_PCLK;
    config.pin_vsync = PIN_CAM_VSYNC;
    config.pin_href = PIN_CAM_HREF;
    config.pin_sccb_sda = PIN_CAM_SIOD;
    config.pin_sccb_scl = PIN_CAM_SIOC;
    config.pin_pwdn = -1;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_QQVGA; // 160x120 for fast real-time computer vision
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[CameraAI] Init failed: 0x%x\n", err);
        return false;
    }

    // Allocate memory in PSRAM for frame difference buffer
    prevFrameSize = 160 * 120;
    prevFrameBuffer = (uint8_t*)ps_malloc(prevFrameSize);
    if (prevFrameBuffer) {
        memset(prevFrameBuffer, 0, prevFrameSize);
    }

    initialized = true;
    Serial.println("[CameraAI] OV2640 Vision Initialized in PSRAM.");
    return true;
}

FocusAnalysisResult CameraAI::updateFocusMonitoring() {
    FocusAnalysisResult res;
    res.personPresent = true;
    res.isProcrastinating = false;
    res.motionScore = 0.0f;
    res.continuousFocusSec = continuousFocusSec;

    if (!initialized || !prevFrameBuffer) return res;

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) return res;

    // Optical frame difference calculation
    uint32_t diffSum = 0;
    uint32_t activePixels = 0;
    size_t len = min(fb->len, prevFrameSize);

    for (size_t i = 0; i < len; i += 4) { // Subsampled 4x for speed
        int diff = abs((int)fb->buf[i] - (int)prevFrameBuffer[i]);
        if (diff > 25) {
            diffSum += diff;
            activePixels++;
        }
    }

    // Copy to prev buffer
    memcpy(prevFrameBuffer, fb->buf, len);
    esp_camera_fb_return(fb);

    float motionRatio = (float)activePixels / (len / 4.0f);
    res.motionScore = motionRatio * 100.0f;

    uint32_t now = millis();
    if (now - lastCheckTime >= 1000) {
        lastCheckTime = now;

        // Presence & Procrastination Heuristics:
        // Case 1: Extreme stillness or zero activity for extended time -> User walked away
        if (motionRatio < 0.005f) {
            if (absenceStartTime == 0) absenceStartTime = now;
            else if (now - absenceStartTime > 12000) { // 12 seconds absent
                res.personPresent = false;
                res.isProcrastinating = true;
                continuousFocusSec = max(0, continuousFocusSec - 2);
            }
        } 
        // Case 2: Very high chaotic motion -> User waving, phone handling, or distracted
        else if (motionRatio > 0.45f) {
            absenceStartTime = 0;
            res.isProcrastinating = true;
        } 
        // Case 3: Steady subtle work motion -> Typing, reading, taking notes
        else {
            absenceStartTime = 0;
            continuousFocusSec++;
            res.personPresent = true;
            res.isProcrastinating = false;
        }
    }

    res.continuousFocusSec = continuousFocusSec;
    return res;
}

camera_fb_t* CameraAI::captureSnapshot() {
    if (!initialized) return nullptr;
    return esp_camera_fb_get();
}

void CameraAI::returnSnapshot(camera_fb_t* fb) {
    if (fb) esp_camera_fb_return(fb);
}
