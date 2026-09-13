#pragma once
#include <Arduino.h>
#include "pinout.h"

class BatteryMonitor {
public:
    BatteryMonitor();
    bool init();
    void update();
    float getVoltage();
    int getPercentage();
    bool isLowBattery();

private:
    float filteredVoltage;
    int percentage;
    uint32_t lastReadTime;
};
