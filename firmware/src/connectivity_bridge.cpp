#include "connectivity_bridge.h"

ConnectivityBridge::ConnectivityBridge() 
    : wifiOnline(false), lastWifiCheckMs(0),
      bleConnected(false), pServer(nullptr), pTxChar(nullptr),
      pRxChar(nullptr), pLlmChar(nullptr), hasNewLlmResponse(false) {}

bool ConnectivityBridge::init() {
    // 1. Iniciar conexión Wi-Fi directa
    Serial.printf("[Connectivity] Conectando a Wi-Fi: %s ...\n", FOCUSBOT_WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(FOCUSBOT_WIFI_SSID, FOCUSBOT_WIFI_PASSWORD);

    // Esperar hasta 5 segundos para conexión rápida sin bloquear el arranque
    uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < 4500) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        wifiOnline = true;
        Serial.printf("[Connectivity] Wi-Fi Conectado! IP: %s | RSSI: %d dBm\n", 
                      WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else {
        wifiOnline = false;
        Serial.println("[Connectivity] Wi-Fi en segundo plano (reconectará automáticamente).");
    }

    // 2. Iniciar BLE Companion como canal alternativo
    BLEDevice::init(BLE_DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    BLEService* pService = pServer->createService(SERVICE_UUID);

    pTxChar = pService->createCharacteristic(
        CHAR_TELEMETRY_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxChar->addDescriptor(new BLE2902());

    pRxChar = pService->createCharacteristic(
        CHAR_COMMAND_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pRxChar->setCallbacks(this);

    pLlmChar = pService->createCharacteristic(
        CHAR_LLM_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pLlmChar->setCallbacks(this);

    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    return true;
}

bool ConnectivityBridge::isWifiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void ConnectivityBridge::update() {
    uint32_t now = millis();
    if (now - lastWifiCheckMs > 10000) {
        lastWifiCheckMs = now;
        wifiOnline = (WiFi.status() == WL_CONNECTED);
    }
}

// -----------------------------------------------------------------------------
// Direct Cloud LLM Query
// -----------------------------------------------------------------------------
String ConnectivityBridge::queryCloudLlm(const String& userPrompt) {
    if (!isWifiConnected()) {
        Serial.println("[LLM] Error: Wi-Fi no conectado. No se puede consultar API.");
        return "[WORRIED][SHAKE] *Bip?* No tengo señal Wi-Fi para pensar...";
    }

    String provider = String(LLM_PROVIDER);
    provider.toUpperCase();

    if (provider == "OPENAI") {
        return callOpenAI(userPrompt);
    } else if (provider == "GEMINI") {
        return callGemini(userPrompt);
    } else if (provider == "CLAUDE") {
        return callClaude(userPrompt);
    } else if (provider == "OLLAMA_LOCAL") {
        return callOllama(userPrompt);
    } else {
        return callOpenAI(userPrompt);
    }
}

// -----------------------------------------------------------------------------
// OpenAI API (gpt-4o-mini / gpt-3.5-turbo)
// -----------------------------------------------------------------------------
String ConnectivityBridge::callOpenAI(const String& prompt) {
    WiFiClientSecure client;
    client.setInsecure(); // Permite SSL sin inflar la flash con certificados CA
    HTTPClient http;

    if (!http.begin(client, "https://api.openai.com/v1/chat/completions")) {
        return "[WORRIED] *Bip!* Error de conexion SSL con OpenAI.";
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + OPENAI_API_KEY);
    http.setTimeout(12000);

    JsonDocument doc;
    doc["model"] = OPENAI_MODEL;
    doc["max_tokens"] = 120;
    doc["temperature"] = 0.7;

    JsonArray messages = doc["messages"].to<JsonArray>();
    
    JsonObject sysMsg = messages.add<JsonObject>();
    sysMsg["role"] = "system";
    sysMsg["content"] = FOCUSBOT_SYSTEM_PROMPT;

    JsonObject userMsg = messages.add<JsonObject>();
    userMsg["role"] = "user";
    userMsg["content"] = prompt;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    String responseText = "";

    if (httpCode == 200) {
        String payload = http.getString();
        JsonDocument resDoc;
        DeserializationError err = deserializeJson(resDoc, payload);
        if (!err) {
            const char* reply = resDoc["choices"][0]["message"]["content"];
            if (reply) responseText = String(reply);
        }
    } else {
        Serial.printf("[OpenAI] HTTP Error: %d\n", httpCode);
        responseText = "[WORRIED] *Bip error " + String(httpCode) + "*";
    }

    http.end();
    return responseText;
}

// -----------------------------------------------------------------------------
// Google Gemini API (gemini-1.5-flash)
// -----------------------------------------------------------------------------
String ConnectivityBridge::callGemini(const String& prompt) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
                 String(GEMINI_MODEL) + ":generateContent?key=" + String(GEMINI_API_KEY);

    if (!http.begin(client, url)) {
        return "[WORRIED] *Bip!* Error de conexion SSL con Gemini.";
    }

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(12000);

    JsonDocument doc;
    JsonArray contents = doc["contents"].to<JsonArray>();
    JsonObject part = contents.add<JsonObject>()["parts"].to<JsonArray>().add<JsonObject>();
    part["text"] = String(FOCUSBOT_SYSTEM_PROMPT) + "\n\nHumano dice: " + prompt;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    String responseText = "";

    if (httpCode == 200) {
        String payload = http.getString();
        JsonDocument resDoc;
        DeserializationError err = deserializeJson(resDoc, payload);
        if (!err) {
            const char* reply = resDoc["candidates"][0]["content"]["parts"][0]["text"];
            if (reply) responseText = String(reply);
        }
    } else {
        Serial.printf("[Gemini] HTTP Error: %d\n", httpCode);
        responseText = "[WORRIED] *Bip Gemini error " + String(httpCode) + "*";
    }

    http.end();
    return responseText;
}

// -----------------------------------------------------------------------------
// Anthropic Claude API (claude-3-5-haiku)
// -----------------------------------------------------------------------------
String ConnectivityBridge::callClaude(const String& prompt) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    if (!http.begin(client, "https://api.anthropic.com/v1/messages")) {
        return "[WORRIED] *Bip!* Error de conexion SSL con Claude.";
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", CLAUDE_API_KEY);
    http.addHeader("anthropic-version", "2023-06-01");
    http.setTimeout(12000);

    JsonDocument doc;
    doc["model"] = CLAUDE_MODEL;
    doc["max_tokens"] = 120;
    doc["system"] = FOCUSBOT_SYSTEM_PROMPT;

    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject userMsg = messages.add<JsonObject>();
    userMsg["role"] = "user";
    userMsg["content"] = prompt;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    String responseText = "";

    if (httpCode == 200) {
        String payload = http.getString();
        JsonDocument resDoc;
        DeserializationError err = deserializeJson(resDoc, payload);
        if (!err) {
            const char* reply = resDoc["content"][0]["text"];
            if (reply) responseText = String(reply);
        }
    } else {
        Serial.printf("[Claude] HTTP Error: %d\n", httpCode);
        responseText = "[WORRIED] *Bip Claude error " + String(httpCode) + "*";
    }

    http.end();
    return responseText;
}

// -----------------------------------------------------------------------------
// Local Ollama Server (http://HOST:PORT/api/generate)
// -----------------------------------------------------------------------------
String ConnectivityBridge::callOllama(const String& prompt) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://" + String(LOCAL_LLM_HOST) + ":" + String(LOCAL_LLM_PORT) + "/api/generate";

    if (!http.begin(client, url)) {
        return "[WORRIED] *Bip!* No encuentro servidor Ollama local.";
    }

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(15000);

    JsonDocument doc;
    doc["model"] = LOCAL_LLM_MODEL;
    doc["prompt"] = String(FOCUSBOT_SYSTEM_PROMPT) + "\n\nHumano: " + prompt;
    doc["stream"] = false;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    String responseText = "";

    if (httpCode == 200) {
        String payload = http.getString();
        JsonDocument resDoc;
        DeserializationError err = deserializeJson(resDoc, payload);
        if (!err) {
            const char* reply = resDoc["response"];
            if (reply) responseText = String(reply);
        }
    } else {
        Serial.printf("[Ollama] HTTP Error: %d\n", httpCode);
        responseText = "[WORRIED] *Bip Ollama error " + String(httpCode) + "*";
    }

    http.end();
    return responseText;
}

// -----------------------------------------------------------------------------
// BLE Callbacks
// -----------------------------------------------------------------------------
void ConnectivityBridge::onConnect(BLEServer* server) {
    bleConnected = true;
    Serial.println("[Connectivity] App Móvil Conectada por BLE.");
}

void ConnectivityBridge::onDisconnect(BLEServer* server) {
    bleConnected = false;
    Serial.println("[Connectivity] App Desconectada. Reanudando BLE...");
    BLEDevice::startAdvertising();
}

void ConnectivityBridge::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() == 0) return;

    if (pCharacteristic == pRxChar) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, value);
        if (!err) {
            const char* cmd = doc["cmd"];
            Serial.printf("[BLE] Comando App: %s\n", cmd);
        }
    } else if (pCharacteristic == pLlmChar) {
        lastLlmResponse = String(value.c_str());
        hasNewLlmResponse = true;
        Serial.printf("[BLE] Mensaje LLM: %s\n", lastLlmResponse.c_str());
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
