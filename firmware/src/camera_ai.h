#pragma once
// =============================================================================
// camera_ai.h  —  FocusBot Vision & AI Analysis Module
// =============================================================================
// Wraps the ESP32-CAM OV2640 pipeline.  Performs frame-differencing to detect
// motion/presence, estimates a look-target centroid for eye-tracking, and
// applies a simple blob-size heuristic for face detection.
// =============================================================================
#include <Arduino.h>
#include "esp_camera.h"
#include "pinout.h"

// ---------------------------------------------------------------------------
// Result struct returned by updateFocusMonitoring() every call.
// ---------------------------------------------------------------------------
struct FocusAnalysisResult {
    bool  personPresent;       // True when meaningful motion detected
    bool  faceDetected;        // Blob-size heuristic: face-sized region of motion
    bool  isProcrastinating;   // True when motion suggests off-task behaviour
    float motionScore;         // 0..100  — magnitude of inter-frame diff
    int   continuousFocusSec;  // Seconds of uninterrupted on-task presence
    float lookTargetX;         // -1..+1  centroid of active pixels (horizontal)
    float lookTargetY;         // -1..+1  centroid of active pixels (vertical)
};

// ---------------------------------------------------------------------------
// CameraAI  —  singleton-style object; create one globally in main.cpp
// ---------------------------------------------------------------------------
class CameraAI {
public:
    CameraAI();

    /// Initialise camera hardware. Returns true on success.
    bool init();

    /// Call every loop iteration (or on a timer).  Returns the latest analysis.
    FocusAnalysisResult updateFocusMonitoring();

    /// Grab a raw JPEG snapshot (caller must call returnSnapshot when done).
    camera_fb_t* captureSnapshot();

    /// Return a frame-buffer obtained from captureSnapshot().
    void returnSnapshot(camera_fb_t* fb);

private:
    bool     initialized;

    // Frame-diff buffers (QQQVGA 160×120 grayscale)
    uint8_t* prevFrameBuffer;
    size_t   prevFrameSize;

    // Timing
    uint32_t lastCheckTime;
    uint32_t absenceStartTime;

    // Running counters
    int   continuousFocusSec;
    int   procrastinationWarnings;

    // Smoothed look-target (exponential moving average)
    float smoothLookX;
    float smoothLookY;
};
