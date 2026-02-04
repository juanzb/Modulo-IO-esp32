#include "connect_wifi.hpp"

/* ================== CONSTRUCTOR ================== */
WiFiManager::WiFiManager() {
    config.deviceID = (uint32_t)ESP.getEfuseMac();
    config.ssid = "ZARACHE BARRIOS";
    config.pass = "z4r4ch3b4rr10s";
}

WiFiManager& WiFiManager::instance() {
    static WiFiManager instance;
    return instance;
}

/* ================== INICIALIZACIÓN ================== */
void WiFiManager::begin() {
    Serial.println("[WiFiManager] Iniciando WiFiManager...");

    // Montar LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("[WiFiManager] Error al montar LittleFS!");
        return;
    }
    Serial.println("[WiFiManager] LittleFS montado correctamente.");

    // Modo dual por seguridad
    WiFi.mode(WIFI_AP_STA);

    // Cargar configuración previa
    loadConfig();

    // Lógica de arranque
    if (config.ssid.isEmpty() || config.apMode) {
        Serial.println("[WiFiManager] No hay red guardada o AP forzado -> iniciar AP.");
        startAccessPoint("12345678");
    } else {
        Serial.printf("[WiFiManager] Intentando conectar a SSID guardado: %s\n", config.ssid.c_str());
        if (!connectToWiFi()) {
            Serial.println("[WiFiManager] No se pudo conectar, activando AP.");
            startAccessPoint("12345678");
        }
    }
}

/* ================== LOOP ================== */
void WiFiManager::loop() {
    updateConnection();
}


/* ================== CONTROL AP ================== */
bool WiFiManager::startAccessPoint(const String& apPassword) {
    String apName = getAPName();
    Serial.printf("[WiFiManager] Iniciando AP: %s\n", apName.c_str());

    WiFi.mode(WIFI_AP_STA);
    bool ok = WiFi.softAP(apName.c_str(), apPassword.c_str());
    if (ok) {
        config.apMode = true;
        saveConfig();
        Serial.printf("[WiFiManager] AP activo | IP: %s\n", WiFi.softAPIP().toString().c_str());
    } else {
        Serial.println("[WiFiManager] Error al iniciar AP");
    }
    return ok;
}

void WiFiManager::stopAccessPoint() {
    if (WiFi.softAPdisconnect(true)) {
        Serial.println("[WiFiManager] AP deshabilitado");
        config.apMode = false;
        saveConfig();
    } else {
        Serial.println("[WiFiManager] Error al desactivar AP");
    }
}

/* ================== CAMBIO DE RED ================== */
bool WiFiManager::changeNetwork(const String& ssid, const String& pass) {
    Serial.printf("[WiFiManager] Cambiando red a: %s\n", ssid.c_str());
    config.ssid = ssid;
    config.pass = pass;
    config.apMode = false;

    if (!saveConfig()) {
        Serial.println("[WiFiManager] Error guardando configuración");
        return false;
    }

    if (!connectToWiFi()) {
        Serial.println("[WiFiManager] No se pudo conectar a la nueva red. Activando AP...");
        startAccessPoint("12345678");
        return false;
    }

    stopAccessPoint();
    return true;
}

/* ================== CONEXIÓN / RECONEXIÓN ================== */
bool WiFiManager::initWiFi() {
    WiFi.begin(config.ssid.c_str(), config.pass.c_str());
    unsigned long start = millis();
    Serial.printf("[WiFiManager] Conectando a %s ...\n", config.ssid.c_str());
    while (WiFi.status() != WL_CONNECTED && millis() - start < CONNECT_TIMEOUT_MS) {
        delay(100);
        yield();
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WiFiManager] Conectado. IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }

    Serial.println("[WiFiManager] Falló conexión WiFi.");
    WiFi.disconnect();
    return false;
}

bool WiFiManager::connectToWiFi() {
    if (config.ssid.isEmpty()) {
        Serial.println("[WiFiManager] SSID vacío, iniciando AP...");
        startAccessPoint("12345678");
        return false;
    }

    if (initWiFi()) {
        config.apMode = false;
        saveConfig();
        return true;
    }

    startAccessPoint("12345678");
    return false;
}

void WiFiManager::updateConnection() {
    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt < RECONNECT_DELAY_MS) return;
    lastAttempt = millis();

    if (WiFi.status() != WL_CONNECTED && !config.apMode && !reconnecting) {
        reconnecting = true;
        Serial.println("[WiFiManager] WiFi desconectado. Intentando reconectar...");
        connectToWiFi();
        reconnecting = false;
    }
}

void WiFiManager::forceReconnect() {
    WiFi.disconnect(true);
    delay(100);
    connectToWiFi();
}


/* ================== PERSISTENCIA ================== */
void WiFiManager::loadConfig() {
    if (!LittleFS.exists(CONFIG_PATH)) {
        Serial.println("[WiFiManager] No existe config.json, creando configuración por defecto.");
        config.apMode = true;
        config.ssid = "";
        config.pass = "";
        saveConfig();
        return;
    }

    File file = LittleFS.open(CONFIG_PATH, "r");
    if (!file) {
        Serial.println("[WiFiManager] Error abriendo config.json");
        config.apMode = true;
        return;
    }

    JsonDocument doc;
    if (deserializeJson(doc, file)) {
        Serial.println("[WiFiManager] Error leyendo config.json, usando defaults.");
        config.apMode = true;
        return;
    }
    file.close();

    config.ssid = doc["ssid"] | "";
    config.pass = doc["pass"] | "";
    config.apMode = doc["apMode"] | config.ssid.isEmpty();
    config.deviceID = doc["deviceID"] | (uint32_t)ESP.getEfuseMac();

    Serial.printf("[WiFiManager] Configuración cargada. SSID: %s, AP: %s\n",
                  config.ssid.c_str(),
                  config.apMode ? "true" : "false");
}

bool WiFiManager::saveConfig() {
    File file = LittleFS.open(CONFIG_PATH, "w");
    if (!file) {
        Serial.println("[WiFiManager] Error abriendo config.json para escritura.");
        return false;
    }

    JsonDocument doc;
    doc["apMode"] = config.apMode;
    doc["ssid"] = config.ssid;
    doc["pass"] = config.pass;
    doc["deviceID"] = config.deviceID;

    if (serializeJson(doc, file) == 0) {
        Serial.println("[WiFiManager] Error escribiendo config.json");
        file.close();
        return false;
    }

    file.close();
    return true;
}


/* ================== UTILIDADES ================== */
String WiFiManager::getAPName() {
    return "ESP32-" + String(config.deviceID, HEX);
}

IPAddress WiFiManager::getLocalIP() {
  return WiFi.localIP();
}

bool WiFiManager::isAPActive() {
    return config.apMode;
}

WiFiConfig WiFiManager::getConfig() {
    return config;
}

