#include "connectivity_bridge.h"

ConnectivityBridge::ConnectivityBridge() 
    : bleConnected(false), pServer(nullptr), pTxChar(nullptr),
      pRxChar(nullptr), pLlmChar(nullptr), hasNewLlmResponse(false) {}

bool ConnectivityBridge::init() {
    BLEDevice::init("FocusBot-Companion");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    BLEService* pService = pServer->createService(SERVICE_UUID);

    // Telemetry Tx (Notify)
    pTxChar = pService->createCharacteristic(
        CHAR_TELEMETRY_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxChar->addDescriptor(new BLE2902());

    // Command Rx (Write)
    pRxChar = pService->createCharacteristic(
        CHAR_COMMAND_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pRxChar->setCallbacks(this);

    // LLM Stream Rx (Write)
    pLlmChar = pService->createCharacteristic(
        CHAR_LLM_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pLlmChar->setCallbacks(this);

    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // iPhone connection optimization
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("[Connectivity] BLE Advertising started as 'FocusBot-Companion'");
    return true;
}

void ConnectivityBridge::onConnect(BLEServer* server) {
    bleConnected = true;
    Serial.println("[Connectivity] Mobile App Connected via BLE!");
}

void ConnectivityBridge::onDisconnect(BLEServer* server) {
    bleConnected = false;
    Serial.println("[Connectivity] Mobile App Disconnected. Restarting advertising...");
    BLEDevice::startAdvertising();
}

void ConnectivityBridge::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() == 0) return;

    if (pCharacteristic == pRxChar) {
        // Parse incoming JSON command
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, value);
        if (!err) {
            const char* cmd = doc["cmd"];
            Serial.printf("[Connectivity] App Command Received: %s\n", cmd);
        }
    } else if (pCharacteristic == pLlmChar) {
        lastLlmResponse = String(value.c_str());
        hasNewLlmResponse = true;
        Serial.printf("[Connectivity] LLM Text Received: %s\n", lastLlmResponse.c_str());
    }
}

bool ConnectivityBridge::isConnected() {
    return bleConnected;
}

void ConnectivityBridge::sendTelemetry(RobotMode mode, int batteryPct, int focusScore) {
    if (!bleConnected || !pTxChar) return;

    JsonDocument doc;
    doc["mode"] = (int)mode;
    doc["battery"] = batteryPct;
    doc["focus"] = focusScore;

    char buffer[128];
    size_t len = serializeJson(doc, buffer);
    pTxChar->setValue((uint8_t*)buffer, len);
    pTxChar->notify();
}

String ConnectivityBridge::pollIncomingLlmMessage() {
    if (hasNewLlmResponse) {
        hasNewLlmResponse = false;
        return lastLlmResponse;
    }
    return "";
}

void ConnectivityBridge::update() {
    // Keep-alive or periodic sync
}
