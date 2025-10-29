#include "connect_wifi.hpp"

WiFiManager::WiFiManager() {
    config.deviceID = (uint32_t)ESP.getEfuseMac();
}

void WiFiManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("[WiFiManager] Error montando LittleFS");
        return;
    }

    loadConfig();

    if (config.apMode || config.ssid.isEmpty()) {
        startAccessPoint();
    } else {
        connectToWiFi();
    }
}

bool WiFiManager::initWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFiManager] Ya conectado a WiFi");
        return true;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid.c_str(), config.pass.c_str());

    unsigned long start = millis();
    const unsigned long timeout = 10000; // 10 segundos

    Serial.printf("[WiFiManager] Conectando a SSID: %s ...\n", config.ssid.c_str());

    while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
        delay(250);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n[WiFiManager] Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
        config.apMode = false;
        return true;
    } else {
        Serial.println("\n[WiFiManager] No se pudo conectar, activando AP...");
        startAccessPoint();
        return false;
    }
}

#include <ArduinoJson.h>

String WiFiManager::scanNetworks() {
    WiFi.scanDelete();
    int n = WiFi.scanNetworks();

    DynamicJsonDocument doc(1024);
    JsonArray arr = doc.to<JsonArray>();

    for (int i = 0; i < n; ++i) {
        JsonObject net = arr.createNestedObject();
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["encryption"] = WiFi.encryptionType(i);
    }

    if (n == 0) {
        Serial.println("[WiFiManager] No se encontraron redes");
    } else {
        Serial.printf("[WiFiManager] %d redes encontradas\n", n);
    }

    String output;
    serializeJson(doc, output);
    return output;
}


bool WiFiManager::startAccessPoint(const String& apPassword) {
    String apName = getAPName();
    WiFi.mode(WIFI_AP_STA);

    bool ok = WiFi.softAP(apName.c_str(), apPassword.c_str());
    if (ok) {
        Serial.printf("[WiFiManager] AP activado: %s | IP: %s\n", apName.c_str(), WiFi.softAPIP().toString().c_str());
        config.apMode = true;
    } else {
        Serial.println("[WiFiManager] Error al activar AP");
    }
    return ok;
}

void WiFiManager::stopAccessPoint() {
    WiFi.softAPdisconnect(true);
    Serial.println("[WiFiManager] AP desactivado");
    config.apMode = false;
}

bool WiFiManager::connectToWiFi() {
    if (config.ssid.isEmpty()) {
        Serial.println("[WiFiManager] SSID vacío, activando AP...");
        startAccessPoint();
        return false;
    }

    scanNetworks();

    // Verifica si la red está disponible
    bool found = false;
    for (int i = 0; i < WiFi.scanComplete(); ++i) {
        if (WiFi.SSID(i) == config.ssid) {
            found = true;
            break;
        }
    }

    if (!found) {
        Serial.println("[WiFiManager] SSID no encontrado, activando AP...");
        startAccessPoint();
        return false;
    }

    return initWiFi();
}

void WiFiManager::reconnectIfNeeded() {
    if (WiFi.status() != WL_CONNECTED && !config.apMode) {
        Serial.println("[WiFiManager] WiFi desconectado, intentando reconectar...");
        connectToWiFi();
    }
}

bool WiFiManager::changeNetwork(const String& ssid, const String& pass) {
    config.ssid = ssid;
    config.pass = pass;
    config.apMode = false;

    if (!saveConfig()) {
        Serial.println("[WiFiManager] Error guardando nueva configuración");
        return false;
    }

    return connectToWiFi();
}

// --- PRIVATE ---

void WiFiManager::loadConfig() {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("[WiFiManager] No existe config.json, activando AP por defecto");
        config.apMode = true;
        saveConfig();
        return;
    }

    File f = LittleFS.open("/config.json", "r");
    if (!f) {
        Serial.println("[WiFiManager] Error abriendo config.json");
        config.apMode = true;
        return;
    }

    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, f);
    f.close();

    if (err) {
        Serial.println("[WiFiManager] Error leyendo config.json, activando AP");
        config.apMode = true;
        return;
    }

    config.apMode = doc["apMode"] | true;
    config.ssid = doc["ssid"] | "";
    config.pass = doc["pass"] | "";
    config.deviceID = doc["deviceID"] | (uint32_t)ESP.getEfuseMac();
}

bool WiFiManager::saveConfig() {
    File f = LittleFS.open("/config.json", "w");
    if (!f) {
        Serial.println("[WiFiManager] No se pudo abrir config.json para escritura");
        return false;
    }

    DynamicJsonDocument doc(512);
    doc["apMode"] = config.apMode;
    doc["ssid"] = config.ssid;
    doc["pass"] = config.pass;
    doc["deviceID"] = config.deviceID;

    if (serializeJson(doc, f) == 0) {
        Serial.println("[WiFiManager] Error serializando JSON");
        f.close();
        return false;
    }

    f.close();
    Serial.println("[WiFiManager] Configuración guardada");
    return true;
}

String WiFiManager::getAPName() {
    return "ESP32_" + String(config.deviceID, HEX);
}
