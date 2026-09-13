#include "battery_monitor.h"

BatteryMonitor::BatteryMonitor() 
    : filteredVoltage(3.85f), percentage(75), lastReadTime(0) {}

bool BatteryMonitor::init() {
    analogReadResolution(12); // 12-bit ADC (0-4095)
    analogSetAttenuation(ADC_11db); // Full 0 - 3.3V range
    pinMode(PIN_BAT_ADC, INPUT);
    update();
    return true;
}

void BatteryMonitor::update() {
    uint32_t now = millis();
    if (now - lastReadTime < 500) return;
    lastReadTime = now;

    // Read ADC and convert to voltage
    int raw = analogRead(PIN_BAT_ADC);
    float adcVolts = (raw / 4095.0f) * 3.3f;
    float batVolts = adcVolts * BAT_DIVIDER_RATIO;

    // Exponential moving average filter (EMA)
    filteredVoltage = (filteredVoltage * 0.8f) + (batVolts * 0.2f);

    // Calculate percentage (3.2V - 4.2V)
    float pct = ((filteredVoltage - BAT_MIN_VOLTS) / (BAT_MAX_VOLTS - BAT_MIN_VOLTS)) * 100.0f;
    percentage = constrain((int)pct, 0, 100);
}

float BatteryMonitor::getVoltage() {
    return filteredVoltage;
}

int BatteryMonitor::getPercentage() {
    return percentage;
}

bool BatteryMonitor::isLowBattery() {
    return (filteredVoltage < BAT_LOW_THRESHOLD);
}
