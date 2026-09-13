#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "pinout.h"
#include "robot_types.h"

class AudioSynth {
public:
    AudioSynth();
    bool init();
    void play(SoundEffect snd);
    void playTone(float freqHz, int durationMs, float volume = 0.5f);
    void playSweep(float startFreq, float endFreq, int durationMs, float volume = 0.5f);

private:
    bool initialized;
    void sendBuffer(const int16_t* samples, size_t count);
};
