#pragma once
#include <Arduino.h>
#include "esp_camera.h"
#include "pinout.h"

struct FocusAnalysisResult {
    bool personPresent;
    bool isProcrastinating;
    float motionScore;      // 0.0 to 100.0
    int continuousFocusSec; // Seconds of productive focus
};

class CameraAI {
public:
    CameraAI();
    bool init();
    FocusAnalysisResult updateFocusMonitoring();
    camera_fb_t* captureSnapshot();
    void returnSnapshot(camera_fb_t* fb);

private:
    bool initialized;
    uint8_t* prevFrameBuffer;
    size_t prevFrameSize;
    uint32_t lastCheckTime;
    uint32_t absenceStartTime;
    int continuousFocusSec;
    int procrastinationWarnings;
};
