#include "voice_ear.h"
#include <math.h>

#define MIC_SAMPLE_RATE     16000
#define MIC_I2S_PORT        I2S_NUM_1
#define MIC_BUFFER_LEN      512

VoiceEar::VoiceEar() 
    : initialized(false), lastEnergyTime(0), speechStartTime(0),
      consecutiveSpeechFrames(0), tapCooldown(0) {}

bool VoiceEar::init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = MIC_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // INMP441 delivers 24-bit in 32-bit slot
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = MIC_BUFFER_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_BCLK,
        .ws_io_num = PIN_I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = PIN_I2S_MIC_DATA
    };

    esp_err_t err = i2s_driver_install(MIC_I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) return false;

    err = i2s_set_pin(MIC_I2S_PORT, &pin_config);
    if (err != ESP_OK) return false;

    initialized = true;
    Serial.println("[VoiceEar] INMP441 Microphone Initialized on I2S_NUM_1.");
    return true;
}

VoiceEvent VoiceEar::listen() {
    if (!initialized) return VOICE_NONE;

    int32_t rawSamples[MIC_BUFFER_LEN];
    size_t bytesRead = 0;

    esp_err_t res = i2s_read(MIC_I2S_PORT, rawSamples, sizeof(rawSamples), &bytesRead, 10);
    if (res != ESP_OK || bytesRead == 0) return VOICE_NONE;

    int sampleCount = bytesRead / sizeof(int32_t);
    int64_t energySum = 0;
    int32_t peakVal = 0;

    for (int i = 0; i < sampleCount; i++) {
        // INMP441 sample is in MSB 24 bits
        int32_t sample = rawSamples[i] >> 14;
        peakVal = max(peakVal, abs(sample));
        energySum += ((int64_t)sample * sample);
    }

    float rms = sqrt((double)energySum / (double)sampleCount);
    uint32_t now = millis();

    // Micro-Feature: Sharp physical tap on robot head produces transient peak
    if (tapCooldown > 0) tapCooldown--;
    if (peakVal > 28000 && rms < 9000 && tapCooldown == 0) {
        tapCooldown = 15; // Debounce taps
        Serial.println("[VoiceEar] Detected Head Tap / Petting!");
        return VOICE_TAP_HEAD;
    }

    // Voice Activity Threshold
    if (rms > 2500.0f) {
        consecutiveSpeechFrames++;
        if (consecutiveSpeechFrames == 3) {
            speechStartTime = now;
        }
    } else {
        if (consecutiveSpeechFrames >= 3) {
            uint32_t duration = now - speechStartTime;
            consecutiveSpeechFrames = 0;

            // Pattern recognition by utterance cadence:
            // Short burst (150-500ms): Wake word "Hey Focus"
            if (duration >= 150 && duration <= 600) {
                Serial.println("[VoiceEar] Wake-word / Prompt Triggered!");
                return VOICE_WAKEWORD_DETECTED;
            }
            // Medium duration (650-1400ms): "Focus Mode" command
            else if (duration > 650 && duration <= 1400) {
                Serial.println("[VoiceEar] Command: Focus Mode!");
                return VOICE_COMMAND_FOCUS;
            }
            // Longer command: "Default Mode"
            else if (duration > 1400 && duration <= 2200) {
                Serial.println("[VoiceEar] Command: Default Mode!");
                return VOICE_COMMAND_DEFAULT;
            }
        }
        consecutiveSpeechFrames = 0;
    }

    if (consecutiveSpeechFrames > 10) {
        return VOICE_TALKING_ACTIVE;
    }

    return VOICE_NONE;
}
