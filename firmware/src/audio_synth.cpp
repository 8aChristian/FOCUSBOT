#include "audio_synth.h"
#include <math.h>

#define I2S_SAMPLE_RATE     22050
#define I2S_PORT_NUM        I2S_NUM_0
#define BUFFER_SIZE         256

AudioSynth::AudioSynth() : initialized(false) {}

bool AudioSynth::init() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = PIN_I2S_BCLK,
        .ws_io_num = PIN_I2S_WS,
        .data_out_num = PIN_I2S_SPK_DATA,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t err = i2s_driver_install(I2S_PORT_NUM, &i2s_config, 0, NULL);
    if (err != ESP_OK) return false;

    err = i2s_set_pin(I2S_PORT_NUM, &pin_config);
    if (err != ESP_OK) return false;

    initialized = true;
    return true;
}

void AudioSynth::sendBuffer(const int16_t* samples, size_t count) {
    if (!initialized) return;
    size_t bytesWritten = 0;
    i2s_write(I2S_PORT_NUM, samples, count * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
}

void AudioSynth::playTone(float freqHz, int durationMs, float volume) {
    if (!initialized || freqHz <= 0) return;
    
    int totalSamples = (I2S_SAMPLE_RATE * durationMs) / 1000;
    int16_t buffer[BUFFER_SIZE * 2]; // Stereo interleaved
    float phase = 0.0f;
    float phaseInc = (2.0f * PI * freqHz) / I2S_SAMPLE_RATE;
    
    int samplesRendered = 0;
    while (samplesRendered < totalSamples) {
        int chunk = min(BUFFER_SIZE, totalSamples - samplesRendered);
        for (int i = 0; i < chunk; i++) {
            // Apply gentle envelope attack/decay to prevent audio pop
            float env = 1.0f;
            int currentIdx = samplesRendered + i;
            if (currentIdx < 200) env = (float)currentIdx / 200.0f;
            else if (currentIdx > totalSamples - 200) env = (float)(totalSamples - currentIdx) / 200.0f;
            
            // Soft sine with slight 2nd harmonic for warmth
            float val = sin(phase) * 0.8f + sin(phase * 2.0f) * 0.2f;
            int16_t sample = (int16_t)(val * volume * env * 16000.0f);
            
            buffer[i * 2] = sample;     // Left
            buffer[i * 2 + 1] = sample; // Right
            
            phase += phaseInc;
            if (phase > 2.0f * PI) phase -= 2.0f * PI;
        }
        sendBuffer(buffer, chunk * 2);
        samplesRendered += chunk;
    }
}

void AudioSynth::playSweep(float startFreq, float endFreq, int durationMs, float volume) {
    if (!initialized) return;
    
    int totalSamples = (I2S_SAMPLE_RATE * durationMs) / 1000;
    int16_t buffer[BUFFER_SIZE * 2];
    float phase = 0.0f;
    
    int samplesRendered = 0;
    while (samplesRendered < totalSamples) {
        int chunk = min(BUFFER_SIZE, totalSamples - samplesRendered);
        for (int i = 0; i < chunk; i++) {
            float progress = (float)(samplesRendered + i) / (float)totalSamples;
            float curFreq = startFreq + (endFreq - startFreq) * progress;
            float phaseInc = (2.0f * PI * curFreq) / I2S_SAMPLE_RATE;
            
            float env = 1.0f;
            int currentIdx = samplesRendered + i;
            if (currentIdx < 150) env = (float)currentIdx / 150.0f;
            else if (currentIdx > totalSamples - 150) env = (float)(totalSamples - currentIdx) / 150.0f;
            
            float val = sin(phase);
            int16_t sample = (int16_t)(val * volume * env * 16000.0f);
            
            buffer[i * 2] = sample;
            buffer[i * 2 + 1] = sample;
            
            phase += phaseInc;
            if (phase > 2.0f * PI) phase -= 2.0f * PI;
        }
        sendBuffer(buffer, chunk * 2);
        samplesRendered += chunk;
    }
}

void AudioSynth::play(SoundEffect snd) {
    switch (snd) {
        case SND_WAKEUP_CHIME:
            // Ascending triad wake up: C5, E5, G5, C6
            playTone(523.25f, 90, 0.4f);
            playTone(659.25f, 90, 0.45f);
            playTone(783.99f, 100, 0.5f);
            playTone(1046.50f, 220, 0.6f);
            break;
            
        case SND_HAPPY_CHIRP:
            // Cute Wall-E chirp
            playSweep(600.0f, 1200.0f, 80, 0.5f);
            delay(20);
            playSweep(1200.0f, 1800.0f, 110, 0.55f);
            break;
            
        case SND_FOCUS_START:
            // Zen temple focus chime: low resonance gong
            playTone(432.0f, 400, 0.6f);
            playTone(864.0f, 250, 0.3f);
            break;
            
        case SND_PROCRASTINATION_ALERT:
            // Friendly warning: double low buzz
            playSweep(300.0f, 220.0f, 150, 0.7f);
            delay(60);
            playSweep(260.0f, 180.0f, 200, 0.75f);
            break;
            
        case SND_CONFIRM_BOOP:
            playTone(880.0f, 60, 0.4f);
            playTone(1320.0f, 90, 0.5f);
            break;
            
        case SND_PETTED_PURR:
            for (int i = 0; i < 4; i++) {
                playSweep(220.0f, 280.0f, 50, 0.3f);
                playSweep(280.0f, 200.0f, 60, 0.3f);
            }
            break;
            
        case SND_SAD_WHINE:
            playSweep(900.0f, 400.0f, 350, 0.4f);
            break;
            
        default:
            break;
    }
}
