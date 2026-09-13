#pragma once
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "robot_types.h"

// UUIDs for FocusBot BLE Service
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_TELEMETRY_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHAR_COMMAND_UUID   "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"
#define CHAR_LLM_RX_UUID    "d29ae63e-b5c6-4b8b-bb8c-8be95d2c2089"

class ConnectivityBridge : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    ConnectivityBridge();
    bool init();
    void update();
    bool isConnected();
    void sendTelemetry(RobotMode mode, int batteryPct, int focusScore);
    String pollIncomingLlmMessage();

    // BLE Callbacks
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;

private:
    bool bleConnected;
    BLEServer* pServer;
    BLECharacteristic* pTxChar;
    BLECharacteristic* pRxChar;
    BLECharacteristic* pLlmChar;
    String lastLlmResponse;
    bool hasNewLlmResponse;
};
