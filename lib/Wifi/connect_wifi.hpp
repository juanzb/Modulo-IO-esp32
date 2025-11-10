#pragma once

/*
 * connect_wifi.hpp
 *
 * Interfaz pública de WiFiManager para ESP32
 *
 * Requisitos cubiertos:
 *  - Habilitar/deshabilitar AP manualmente vía API HTTP
 *  - Conectarse automáticamente a la red guardada tras reinicio
 *  - Manejo de reconexión automática
 *  - Escaneo de redes (expuesto por endpoint, seguro contra deadlocks)
 *  - Persistencia de configuración en LittleFS (/config.json)
 *  - API segura para integración con AsyncWebServer (pausar/reanudar si es necesario)
 *
 * Diseño:
 *  - Singleton (WiFiManager::instance()) para que la API HTTP use siempre
 *    la misma instancia compartida y no cree copias temporales.
 *  - Escaneo realizado en tarea FreeRTOS para no bloquear loop()
 *  - Protección simple de concurrencia con portMUX (spinlock)
 *
 * NOTA: Implementación en connect_wifi.cpp (siguiente paso).
 */

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>

#ifdef USE_ASYNC_SERVER
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif


/* Estado del escaneo */
enum class ScanStatus : uint8_t {
    STOP,
    SCANNING,
    SUCCESS,
    FAILED
};

/* Estructura de configuración que se almacena en LittleFS */
struct WiFiConfig {
    bool apMode = true;         // si true, iniciar en AP si no hay SSID válido
    String ssid = "";           // SSID guardado
    String pass = "";           // Pass guardado
    uint32_t deviceID = 0;      // ID del dispositivo (por ejemplo ESP.getEfuseMac())
};

/* Callback types (opcionales) */
using ScanCompleteCallback = std::function<void(ScanStatus)>;
using ConnectionCallback   = std::function<void(bool connected)>;

class WiFiManager {
public:
    /* Singleton: obtener instancia global */
    static WiFiManager& instance();

    /* Inicialización */
    void begin();               // Monta LittleFS, carga config, arranca AP o intenta conectar
    void loop();                // Llamar desde loop() -> gestion interna ligera

    /* --- Server (opcional) -----------------------------------------
     * Si usas AsyncWebServer, pásalo para permitir que WiFiManager
     * pause/reanude el servidor al hacer escaneos que puedan causar
     * conflictos con AsyncTCP/lwIP.
     * Si no lo usas, simplemente no lo adjuntes.
     */
#ifdef USE_ASYNC_SERVER
    void attachServer(AsyncWebServer* serverPtr);
#endif

    /* --- Punto de acceso (AP) ------------------------------------- */
    // Inicia punto de acceso. Devuelve true si fue activado correctamente.
    bool startAccessPoint(const String& apPassword = "");
    // Desactiva AP. No borra configuración de apMode, solo apaga la radio AP.
    void stopAccessPoint();

    /* --- Conexión / red ------------------------------------------- */
    // Cambia la red (guarda en LittleFS y intenta conectar). Retorna true si la conexión fue exitosa.
    bool changeNetwork(const String& ssid, const String& pass);

    // Forzar reintento de conexión (útil desde API)
    void forceReconnect();

    // Obtener estado actual de la configuración en RAM
    WiFiConfig getConfig();

    /* --- Escaneo de redes ---------------------------------------- */
    // Lanza tarea FreeRTOS que ejecuta el escaneo de forma segura (no bloqueante para loop)
    // startScanAsyncTask() no retorna resultados; llama a consumeScanResults().
    void startScanAsyncTask();

    // (Interno/expuesto para pruebas) Escaneo bloqueante seguro. Debe ejecutarse
    // en una tarea separada si no quieres bloquear loop().
    void startScanNetworks();

    // Estado del último escaneo (STOP/SCANNING/SUCCESS/FAILED)
    ScanStatus getScanState();

    // Consume resultados del último escaneo; mueve el vector y resetea estado a STOP.
    // Devuelve vector vacío si no hay resultados (o scanState != SUCCESS).
    std::vector<std::map<String, String>> consumeScanResults();

    /* --- Persistencia ------------------------------------------- */
    // Guarda configuración actual (apMode, ssid, pass, deviceID) en LittleFS
    // Devuelve true si se escribió correctamente.
    bool saveConfig();

    // Carga configuración desde LittleFS. Si no existe, deja defaults y crea archivo.
    void loadConfig();

    /* --- Callbacks opcionales ----------------------------------- */
    // Registrar callback que notifica cuando termina un escaneo (o cambia estado)
    void onScanComplete(ScanCompleteCallback cb);
    // Registrar callback que notifica cambios de conexión (connected = true/false)
    void onConnectionChange(ConnectionCallback cb);

    /* --- Utilidades / info -------------------------------------- */
    // Obtener nombre del AP (por defecto "ESP32_<deviceID>")
    String getAPName();
    // Obtener IP actual
    IPAddress getLocalIP();
    // Saber si AP está activo (según estado config/apMode)
    bool isAPActive();

private:
    /* Constructor privado (singleton) */
    WiFiManager();
    ~WiFiManager() = default;

    /* Ocultar copias */
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;

    /* Conexión/gestión interna */
    bool initWiFi();           // intentado seguro de conexión (bloqueante corto)
    bool connectToWiFi();      // lógica para decidir: conectar o levantar AP
    void updateConnection();   // llamado desde loop() para reconexiones

    /* Server control interno (si está adjuntado) */
    void stopServer();
    void startServer();

    /* Helpers para escaneo/threads */
    void scanTaskEntry();      // wrapper que se ejecuta dentro de la tarea
    static void scanTaskAdapter(void* arg);

private:
    /* Estado interno */
    WiFiConfig config;
    ScanStatus scanState = ScanStatus::STOP;
    std::vector<std::map<String, String>> scanNetworksResults;

    /* Pointer opcional al AsyncWebServer */
#ifdef USE_ASYNC_SERVER
    AsyncWebServer* server = nullptr;
    bool serverRunning = false;
#endif

    /* Callbacks */
    ScanCompleteCallback scanCallback = nullptr;
    ConnectionCallback connCallback = nullptr;

    /* Concurrencia */
    portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;

    /* Config file path y parámetros */
    const char* CONFIG_PATH = "/config.json";
    const uint32_t SCAN_TASK_STACK = 4096; // tamaño de pila de la tarea de escaneo
    const UBaseType_t SCAN_TASK_PRIO = 1;  // prioridad tarea de escaneo

    /* Tiempo entre reintentos de conexión (ms) y límites */
    const unsigned long CONNECT_TIMEOUT_MS = 8000;
    const unsigned long RECONNECT_DELAY_MS = 5000;

    /* Bandera interna para evitar reconexiones concurrentes */
    volatile bool reconnecting = false;
};
