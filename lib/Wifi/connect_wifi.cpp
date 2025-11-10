#include "connect_wifi.hpp"

/* ================== SINGLETON ================== */
WiFiManager& WiFiManager::instance() {
    static WiFiManager singleton;
    return singleton;
}

/* ================== CONSTRUCTOR ================== */
WiFiManager::WiFiManager() {
    config.deviceID = (uint32_t)ESP.getEfuseMac();
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

/* ================== SERVER (opcional) ================== */
#ifdef USE_ASYNC_SERVER
void WiFiManager::attachServer(AsyncWebServer* serverPtr) {
    this->server = serverPtr;
    this->serverRunning = true;
}
#endif

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
        if (connCallback) connCallback(true);
        return true;
    }

    Serial.println("[WiFiManager] Falló conexión WiFi.");
    WiFi.disconnect();
    if (connCallback) connCallback(false);
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

/* ================== ESCANEO ================== */
void WiFiManager::startScanAsyncTask() {
    if (scanState == ScanStatus::SCANNING) return;

    xTaskCreatePinnedToCore(
        scanTaskAdapter,
        "scanTask",
        SCAN_TASK_STACK,
        this,
        SCAN_TASK_PRIO,
        nullptr,
        0
    );
}

void WiFiManager::scanTaskAdapter(void* arg) {
  // Lanza la tarea FreeRTOS para no bloquear el loop principal
  xTaskCreatePinnedToCore(
    [](void *param) {
      bool apActive = (WiFi.getMode() & WIFI_AP);
      
      if (!(WiFi.getMode() & WIFI_STA)) {
        WiFi.mode(apActive ? WIFI_AP_STA : WIFI_STA);
      }

      delay(200);
      int n = WiFi.scanNetworks(false, true);

      if (n < 0) {
        Serial.println("[ScanManager] Escaneo fallido (-2)");
      } else {
        Serial.printf("[ScanManager] Escaneo completado. %d redes encontradas\n", n);
        for (int i = 0; i < n; ++i) {
          Serial.printf("  SSID: %s | RSSI: %d | Canal: %d\n",
                        WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
        }
      }

      vTaskDelete(NULL);
    },
    "wifi_scan_task",
    4096, // stack size
    NULL,
    1,    // prioridad baja
    NULL,
    1     // ejecuta en core 1 (donde corre el loop principal)
  );
}

void WiFiManager::scanTaskEntry() {
    startScanNetworks();
}

void WiFiManager::startScanNetworks() {
    if (scanState == ScanStatus::SCANNING) return;

#ifdef USE_ASYNC_SERVER
    if (serverRunning && server) {
        Serial.println("[ScanManager] Deteniendo servidor temporalmente para escanear...");
        stopServer();
    }
#endif

    WiFi.mode(WIFI_AP_STA);
    delay(100);
    scanState = ScanStatus::SCANNING;
    int result = WiFi.scanNetworks(true, false);

    unsigned long start = millis();
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING && millis() - start < 10000) {
        delay(100);
        yield();
    }

    int scanResult = WiFi.scanComplete();
    WiFi.scanDelete();

    portENTER_CRITICAL(&lock);
    scanNetworksResults.clear();
    portEXIT_CRITICAL(&lock);

    if (scanResult <= 0) {
        Serial.printf("[ScanManager] Escaneo fallido (%d)\n", scanResult);
        scanState = ScanStatus::FAILED;
#ifdef USE_ASYNC_SERVER
        if (server) startServer();
#endif
        if (scanCallback) scanCallback(scanState);
        return;
    }

    Serial.printf("[ScanManager] %d redes encontradas.\n", scanResult);

    portENTER_CRITICAL(&lock);
    for (int i = 0; i < scanResult; i++) {
        scanNetworksResults.push_back({
            {"ssid", WiFi.SSID(i)},
            {"rssi", String(WiFi.RSSI(i))},
            {"channel", String(WiFi.channel(i))},
            {"encryption", String(WiFi.encryptionType(i))}
        });
    }
    portEXIT_CRITICAL(&lock);

    scanState = ScanStatus::SUCCESS;
#ifdef USE_ASYNC_SERVER
    if (server) startServer();
#endif
    if (scanCallback) scanCallback(scanState);
}

ScanStatus WiFiManager::getScanState() {
    return scanState;
}

std::vector<std::map<String, String>> WiFiManager::consumeScanResults() {
    if (scanState != ScanStatus::SUCCESS) return {};
    portENTER_CRITICAL(&lock);
    auto result = std::move(scanNetworksResults);
    scanNetworksResults.clear();
    portEXIT_CRITICAL(&lock);
    scanState = ScanStatus::STOP;
    return result;
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

/* ================== CALLBACKS ================== */
void WiFiManager::onScanComplete(ScanCompleteCallback cb) { scanCallback = cb; }
void WiFiManager::onConnectionChange(ConnectionCallback cb) { connCallback = cb; }

/* ================== UTILIDADES ================== */
String WiFiManager::getAPName() {
    return "ESP32_" + String(config.deviceID, HEX);
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

/* ================== SERVER CONTROL INTERNO ================== */
#ifdef USE_ASYNC_SERVER
void WiFiManager::stopServer() {
    if (server && serverRunning) {
        server->end();
        serverRunning = false;
    }
}

void WiFiManager::startServer() {
    if (server && !serverRunning) {
        server->begin();
        serverRunning = true;
    }
}
#endif
