#include "connect_wifi.hpp"
Logger logger("WIFI-MANAGER");

/* ================== CONSTRUCTOR ================== */
WiFiManager::WiFiManager() {
    config.deviceID = (uint32_t)ESP.getEfuseMac();
    config.ssid = "ZARACHE BARRIOS";
    config.pass = "z4r4ch3b4rr10s";
    config.apMode = false;
}

WiFiManager& WiFiManager::instance() {
    static WiFiManager instance;
    return instance;
}

/* ================== INICIALIZACIÓN ================== */
void WiFiManager::begin() {
    logger.log("STARTING...");
    WiFi.mode(WIFI_AP_STA);

    // Cargar configuración previa
    // loadConfig();

    // Lógica de arranque
    if (config.ssid.isEmpty() || config.apMode) {
        startAccessPoint("12345678");
    } else {
        logger.log("RECONNECT TO SSID SAVED: %s\n", config.ssid.c_str());
        if (!connectToWiFi()) {
            logger.log("ERROR TO RECONNECT");
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
    logger.log("STARTING MODE AP: %s\n", apName.c_str());

    WiFi.mode(WIFI_AP_STA);
    bool ok = WiFi.softAP(apName.c_str(), apPassword.c_str());
    if (ok) {
        config.apMode = true;
        // saveConfig();
        logger.log("MODE AP ENABLE | IP: %s\n", WiFi.softAPIP().toString().c_str());
    } else {
        logger.log("ERROR ENABLE MODE AP");
    }
    return ok;
}

void WiFiManager::stopAccessPoint() {
    if (WiFi.softAPdisconnect(true)) {
        logger.log("AP deshabilitado");
        config.apMode = false;
        // saveConfig();
    } else {
        logger.log("Error al desactivar AP");
    }
}

/* ================== CAMBIO DE RED ================== */
bool WiFiManager::changeNetwork(const String& ssid, const String& pass) {
    logger.log("Cambiando red a: %s\n", ssid.c_str());
    config.ssid = ssid;
    config.pass = pass;
    config.apMode = false;

    // if (!saveConfig()) {
    //     logger.log("Error guardando configuración");
    //     return false;
    // }

    if (!connectToWiFi()) {
        logger.log("No se pudo conectar a la nueva red. Activando AP...");
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
    logger.log("CONNECTING TO %s ...\n", config.ssid.c_str());
    while (WiFi.status() != WL_CONNECTED && millis() - start < CONNECT_TIMEOUT_MS) {
        delay(100);
        yield();
    }

    if (WiFi.status() == WL_CONNECTED) {
        logger.log("CONNECTED. IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }

    logger.log("ERROR CONNECTING TO WiFi.");
    WiFi.disconnect();
    return false;
}

bool WiFiManager::connectToWiFi() {
    if (config.ssid.isEmpty()) {
        logger.log("SSID VOID, STARTING AP...");
        startAccessPoint("12345678");
        return false;
    }

    if (initWiFi()) {
        config.apMode = false;
        // saveConfig();
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
        logger.log("WiFi desconectado. Intentando reconectar...");
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
// WiFiManager::loadConfig() {

// }

// WiFiManager::saveConfig() {

// }


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
