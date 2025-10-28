#include <connect_wifi.hpp>
#include <ArduinoJson.h>
#include <LittleFS.h>

WiFiManager::WiFiManager() {}

void WiFiManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Error montando LittleFS");
        return;
    }

    loadConfig();

    if (config.apMode || config.ssid.isEmpty()) {
        startAccessPoint();
    } else {
        connectToWiFi();
    }
}

void WiFiManager::loadConfig() {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("No existe config.json, se usará modo AP por defecto.");
        config.apMode = true;
        config.ssid = "";
        config.pass = "";
        config.deviceID = (uint32_t)ESP.getEfuseMac();
        saveConfig(config.apMode, config.ssid, config.pass, config.deviceID);
        return;
    }

    File f = LittleFS.open("/config.json", "r");
    if (!f) {
        Serial.println("Error abriendo config.json");
        return;
    }

    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, f);
    f.close();

    if (err) {
        Serial.println("Error leyendo config.json, usando modo AP.");
        config.apMode = true;
        return;
    }

    config.apMode = doc["apMode"] | true;
    config.ssid = doc["ssid"] | "";
    config.pass = doc["pass"] | "";
    config.deviceID = doc["deviceID"] | (uint32_t)ESP.getEfuseMac();
}

bool WiFiManager::saveConfig(bool apMode, const String& ssid, const String& pass, int deviceID) {
    config.apMode = apMode;
    config.ssid = ssid;
    config.pass = pass;
    config.deviceID = deviceID;

    Serial.println("\n[SAVECONFIG] INICIANDO GUARDADO DE CONFIGURACIÓN...");

    File f = LittleFS.open("/config.json", "w");
    if (!f) {
        Serial.println("[SAVECONFIG] ERROR: NO SE PUDO ABRIR config.json PARA ESCRITURA");
        return false;
    }

    DynamicJsonDocument doc(512);
    doc["apMode"] = config.apMode;
    doc["ssid"] = config.ssid;
    doc["pass"] = config.pass;
    doc["deviceID"] = config.deviceID;

    if (serializeJson(doc, f) == 0) {
        Serial.println("[SAVECONFIG] ERROR: NO SE PUDO SERIALIZAR JSON");
        f.close();
        return false;
    }

    f.close();
    Serial.println("[SAVECONFIG] CONFIGURACIÓN GUARDADA CORRECTAMENTE ");
    return true;
}


bool WiFiManager::connectToWiFi() {
    Serial.println("\n[CONNECT] INICIANDO PROCESO DE CONEXIÓN...");
    Serial.printf("[CONNECT] SSID: %s\n", config.ssid.c_str());

    // Verifica que exista SSID
    if (config.ssid.isEmpty()) {
        Serial.println("[CONNECT] ERROR: SSID VACÍO — INICIANDO MODO AP");
        startAccessPoint();
        return false;
    }

    // Escanea redes disponibles
    Serial.println("[CONNECT] ESCANEANDO REDES DISPONIBLES...");
    int networkCount = WiFi.scanNetworks();

    if (networkCount <= 0) {
        Serial.println("[CONNECT] ERROR: NO SE DETECTARON REDES");
        startAccessPoint();
        return false;
    }

    bool ssidFound = false;
    for (int i = 0; i < networkCount; i++) {
        if (WiFi.SSID(i) == config.ssid) {
            ssidFound = true;
            break;
        }
    }

    if (!ssidFound) {
        Serial.println("[CONNECT] ERROR: SSID NO ENCONTRADO EN REDES DISPONIBLES");
        startAccessPoint();
        return false;
    }

    Serial.println("[CONNECT] RED DETECTADA, INICIANDO CONEXIÓN...");

    // Asegurar que no haya conexión previa activa
    WiFi.disconnect(true, true);
    delay(300);

    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid.c_str(), config.pass.c_str());

    unsigned long start = millis();
    const unsigned long timeout = 8000; // 8 segundos máx.

    while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
        delay(250);
        yield();
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n[CONNECT] ERROR: NO SE PUDO ESTABLECER CONEXIÓN");
        WiFi.disconnect(true, true);
        delay(200);
        startAccessPoint();
        return false;
    }

    Serial.println("\n[CONNECT] WIFI CONECTADO CORRECTAMENTE ");
    Serial.print("[CONNECT] IP LOCAL: ");
    Serial.println(WiFi.localIP());

    config.apMode = false;
    return true;
}


bool WiFiManager::startAccessPoint(const String& apPassword) {
    String apName = "ESP32_" + String(config.deviceID, HEX);

    // No apagamos WiFi STA para evitar conflicto si ya estaba intentando conectarse
    WiFi.mode(WIFI_AP_STA);

    bool result = WiFi.softAP(apName.c_str(), apPassword.c_str());

    if (result) {
        Serial.printf("[AP] MODO AP ACTIVADO — SSID: %s | IP: %s\n",
                      apName.c_str(), WiFi.softAPIP().toString().c_str());
        config.apMode = true;
    } else {
        Serial.println("[AP] ERROR AL INICIAR MODO AP");
    }

    return result;
}

void WiFiManager::stopAccessPoint() {
    WiFi.softAPdisconnect(true);
    Serial.println("[AP] MODO AP DESACTIVADO");
    config.apMode = false;
}

void WiFiManager::scanNetworks() {
    Serial.println("[SCAN] INICIANDO ESCANEO DE REDES...");

    // Asegurar que no haya escaneos previos activos
    WiFi.scanDelete();

    // Iniciar escaneo en modo asíncrono (no bloquea el CPU)
    if (!WiFi.scanNetworks(true)) {
        Serial.println("[SCAN] ERROR: NO SE PUDO INICIAR EL ESCANEO");
        return;
    }

    unsigned long start = millis();
    const unsigned long timeout = 8000; // 8 segundos máximo

    // Esperar a que el escaneo termine
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
        delay(100);
        yield(); // evita que el watchdog se dispare
        if (millis() - start > timeout) {
            Serial.println("[SCAN] ERROR: ESCANEO TARDÓ DEMASIADO, CANCELANDO...");
            WiFi.scanDelete();
            return;
        }
    }

    int n = WiFi.scanComplete();
    if (n <= 0) {
        Serial.println("[SCAN] NO SE ENCONTRARON REDES DISPONIBLES");
    } else {
        Serial.printf("[SCAN] %d RED(ES) ENCONTRADA(S):\n", n);
        for (int i = 0; i < n; ++i) {
            Serial.printf("   %d: %s (%d dBm)\n", i + 1,
                          WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            delay(10); // micro pausa por estabilidad serial
        }
    }

    WiFi.scanDelete();
    Serial.println("[SCAN] ESCANEO FINALIZADO");
}


void WiFiManager::reconnectIfNeeded() {
    if (WiFi.status() != WL_CONNECTED && !config.apMode) {
        Serial.println("[RECONNECT] WiFi desconectado, esperando nueva configuración o reconexión...");
    }
}
